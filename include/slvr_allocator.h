/**
\file
\brief Определение класса allocator::superK

Заголовочный файл с определением класса allocator::superK. 
Аллокатор позволяет использовать до 1024 байт для 
размещения данных стандартных контейнеров любых типов. 
Выделение памяти для блока происходит 1 раз в начале работы
программы. Размер блока может быть изменен константой 
sizeofblock.
Для каждого уникального типа данных Т можно установить лимит
суммарного количества размещаемых элементов с помощью функции
allocator_obj.set_limit(n) - при условии, что n- больше уже 
размещенного количества элементов и меньше размера общего 
выделенного блока памяти. Для снятия ограничения нужно 
использовать n=0.
*/
#ifndef SLVR_ALLOCATOR_H_
#define SLVR_ALLOCATOR_H_

#include <iostream>
#include <limits>
#include <typeinfo>

namespace slvr
{
    namespace allocator
    {
        constexpr size_t sizeofblock = 1024;

        class membuf
        {
        public:
            membuf()
            {
                pbegin = std::malloc(sizeofblock);
                if (!pbegin)
                {
                    throw std::bad_alloc();
                }
                begin = 0;
                end = sizeofblock;
                ptr = 0;
                used = 0;
            }

            ~membuf()
            {
                std::free(pbegin);
            }

            char *place(std::size_t n, std::size_t bytes_per_obj)
            {
#ifdef DEBUG
                /*FOR DEBUG*/ std::cerr << '\n'
                                        << "ptr before allocate =" << ptr << '\n';
#endif
                if (n * bytes_per_obj > sizeofblock - ptr)
                    throw std::bad_alloc();
                auto p = reinterpret_cast<char *>(pbegin) + ptr;
                ptr += n * bytes_per_obj;
#ifdef DEBUG
                /*FOR DEBUG*/ std::cerr << '\n'
                                        << "ptr after allocate =" << ptr << '\n'
                                        << std::endl;
#endif
                return p;
            }

            void clean(char *p, std::size_t n, std::size_t bytes_per_obj)
            {
#ifdef DEBUG
                    std::cerr << "from clean before ptr = " << ptr << '\n';
                    std::cerr << "from clean before used = " << used << '\n';
#endif                
                char *end_of_object = p + n * bytes_per_obj;
                char *end_of_allocated_block = reinterpret_cast<char *>(pbegin) + ptr;
                if (end_of_object == end_of_allocated_block)
                    ptr = ptr - n * bytes_per_obj;
                if (used == 0)
                    ptr = 0;
#ifdef DEBUG
                    std::cerr << "from clean after ptr = " << ptr << '\n';
                    std::cerr << "from clean after used = " << used << '\n';
#endif                     
                return;
            }

            void construct()
            {
                used++;
            }

            void destroy()
            {
                used--;
#ifdef DEBUG
                std::cerr << "from massive::destroy used = " << used << '\n';
#endif                
            }

        private:
            void *pbegin;
            size_t begin;
            size_t end;
            size_t ptr;
            size_t used;
        };

        static slvr::allocator::membuf buffer{};

        template <typename T>
        struct superK
        {

            using value_type = T;

            using pointer = T *;
            using const_pointer = const T *;
            using reference = T &;
            using const_reference = const T &;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;

            template <typename U>
            struct rebind
            {
                using other = superK<U>;
            };

            superK() = default;
            ~superK() = default;
            superK(const superK &) {}

            template <typename U>
            superK(const superK<U> &) {}

        private:
            static size_t manage_max_n(size_t max_n = 0)
            {
                static size_t m_max_n =
                    std::min<size_t>(std::numeric_limits<size_type>::max(), sizeofblock) / sizeof(T);
#ifdef DEBUG
                std::cerr << "manage_max_n before = " << m_max_n << '\n';
#endif
                if (max_n == 0)
                {
#ifdef DEBUG
                    std::cerr << "manage_max_n after = " << m_max_n << '\n';
#endif
                    return m_max_n;
                }
                if (max_n == sizeofblock)
                {
#ifdef DEBUG
                    std::cerr << "manage_max_n after = " << m_max_n << '\n';
#endif
                    m_max_n = std::min<size_t>(std::numeric_limits<size_type>::max(), sizeofblock)/sizeof(T);
                    return m_max_n;
                }
                m_max_n = max_n;
#ifdef DEBUG
                std::cerr << "manage_max_n after = " << m_max_n << '\n';
#endif
                return m_max_n;
            }
            static signed long long total_allocated(signed long long alloc_change = 0)
            {
                static signed long long m_total_allocated = 0;
#ifdef DEBUG
                std::cerr << "total_allocated and alloc_change before = " << m_total_allocated << " " << alloc_change << '\n';
#endif
                if (alloc_change == 0)
                    return m_total_allocated;
                m_total_allocated += alloc_change;
                if (m_total_allocated < 0)
                    throw std::domain_error("Too many deallocation by allocator of type superK");
                return m_total_allocated;
            }

        public:
            void set_limit(size_t n)
            {
                if (n == 0)
                    {
                        manage_max_n(sizeofblock);
                        return;
                    }
                if (n > sizeofblock / sizeof(T) ||
                    n < static_cast<size_t>(total_allocated()))
                    throw std::domain_error("cannot set limits to allocator superK");
                manage_max_n(n);
                return;
            }
            size_t get_limit() const { return manage_max_n(); }
            size_type max_size() const
            {
                size_t maximum = sizeofblock / sizeof(T);
                if (manage_max_n() != 0)
                    maximum = manage_max_n();
                return maximum;
            }

            T *allocate(std::size_t n)
            {
                if (n * sizeof(T) > sizeofblock ||
                    static_cast<size_t>(total_allocated()) + n > manage_max_n())
                    throw std::bad_alloc();
                auto p = buffer.place(n, sizeof(T));
                total_allocated(n);
                return reinterpret_cast<T *>(p);
            }

            void deallocate(T *p, std::size_t n)
            {
                
                
                buffer.clean(reinterpret_cast<char *>(p), n, sizeof(T));
                total_allocated(-static_cast<signed long long>(n));
            }

            template <typename U, typename... Args>
            void construct(U *p, Args &&... args)
            {
                new (p) U(std::forward<Args>(args)...);
                buffer.construct();
            }

            template <typename U>
            void destroy(U *p)
            {
                p->~U();
                buffer.destroy();
            }
        };

    } // namespace allocator
} // namespace slvr

#endif /* SLVR_ALLOCATOR_H_ */