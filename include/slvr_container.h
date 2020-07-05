/**
\file
\brief Определение класса container::massive

Заголовочный файл с определением класса container::massive. 
Контейнер позволяет записывать и хранить целочисленные данные.
Размещение памяти для контейнера типа massive может определяться
постоянными min_reserve-минимальный резерв памяти для контейнера
и step_koef - шаг-множитель наращивания памяти. 
*/
#ifndef SLVR_CONTAINER_H_
#define SLVR_CONTAINER_H_

#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace slvr
{

    namespace container
    {

        template <typename T,
                  typename A = std::allocator<T>,
                  typename = std::enable_if_t<std::is_integral<T>::value>>
        class massive
        {
        public:
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using allocator_type = A ;
            static const int min_reserve = 10;
            static const int step_koef = 3;

        protected:
            T *start;
            T *finish;
            T *end_of_storage;
            A m_alloc;

        public:
            allocator_type &get_allocator()
            {
                return this->m_alloc;
            }
        private:
            void init_storage()
            {
#ifdef DEBUG
                std::cerr << "allocate from massive::init_storage()" << '\n';
#endif
                start = m_alloc.allocate(min_reserve);
                if (start == 0)
                    throw std::bad_alloc();
                finish = start;
                end_of_storage = start + min_reserve;
                return;
            }

        public:
            massive()
                : start(), finish(), end_of_storage()
            {
                init_storage();
            }

            ~massive()
            {
                for (pointer it = finish; it != start; --it)
                    m_alloc.destroy(it - 1);
                m_alloc.deallocate(start, static_cast<size_t>(end_of_storage - start));
            }
            class iterator : public std::iterator<std::bidirectional_iterator_tag, T, T, const T *, T>
            {
            public:
                T *iter = nullptr;
                explicit iterator(T *num = 0) : iter(num) {}
                iterator &operator++()
                {
                    ++iter;
                    return *this;
                }
                iterator &operator--()
                {
                    --iter;
                    return *this;
                }
                bool operator==(iterator other) const { return iter == other.iter; }
                bool operator!=(iterator other) const { return !(*this == other); }
                T &operator*() { return *iter; }
            };

        private:
            void adjust_capacity(const size_t new_reserve)
            {
                if (new_reserve == static_cast<size_t>(end_of_storage - start))
                    return;
                if (new_reserve < static_cast<size_t>(finish - start))
                    throw std::length_error("too low capacity for data");
#ifdef DEBUG
                std::cerr << "allocate from massive::adjust_capacity()" << '\n';
#endif
                pointer new_start = m_alloc.allocate(new_reserve);
                if (!new_start)
                    throw std::bad_alloc();
                pointer new_finish = new_start;
                if ((finish - start) > 0)
                    new_finish = std::uninitialized_copy_n(start, static_cast<size_t>(finish - start), new_start);

                for (pointer it = finish; it != start; --it)
                    m_alloc.destroy(it - 1);
                m_alloc.deallocate(start, static_cast<size_t>(end_of_storage - start));
                start = new_start;
                finish = new_finish;
                end_of_storage = start + new_reserve;
                return;
            }
            void add_memory()
            {
                size_t reserve_max = m_alloc.max_size();

                size_t new_reserve = static_cast<size_t>(end_of_storage - start);
                if (reserve_max / step_koef > new_reserve)
                    new_reserve = new_reserve * step_koef;
                else
                    new_reserve = reserve_max;
                if (new_reserve <= static_cast<size_t>(end_of_storage - start) )
                    throw std::domain_error("out of allocator memory limit");
                adjust_capacity(new_reserve);
            }

        public:
            void push_back(const value_type &x)
            {
                if (static_cast<size_t>(end_of_storage - finish) < 1)
                    add_memory();
                m_alloc.construct(finish, x);
                ++finish;
                return;
            }
            inline T &operator[](size_t i)
            {
                if (start == finish)
                    throw std::range_error("no elements in container");
                if (i < static_cast<size_t>(finish - start))
                    return *(start + i);
                throw std::range_error("element number out of range");
                return *start;
            }
            iterator begin() const
            {
                return iterator(start);
            }
            iterator end() const
            {
                return iterator(finish);
            }
            size_t size() const { return finish - start; }
            size_t capacity() const { return end_of_storage - start; }
            void resize(const size_t n)
            {
                if (n >= static_cast<size_t>(finish - start))
                    return;

                for (pointer it = finish; it != start + n; --it)
                    m_alloc.destroy(it - 1);
                finish = start + n;
                if (n == 0)
                {
                    adjust_capacity(min_reserve);
                    return;
                }
                long k_capacity = (end_of_storage - start) / n;
                if (k_capacity <= step_koef)
                    return;
                if (n < min_reserve)
                {
                    adjust_capacity(min_reserve);
                    return;
                }
                long k_newcap = ((k_capacity - 1) / step_koef);
                k_newcap *= step_koef;
                size_t new_capacity = (end_of_storage - start) / k_newcap + step_koef;
                adjust_capacity(new_capacity);
                return;
            }
            void reserve(const size_t n)
            {
                if (n <= static_cast<size_t>(end_of_storage - start))
                    return;
                adjust_capacity(n);
                return;
            }
        };

    } // namespace container

} // namespace slvr

#endif /* SLVR_CONTAINER_H_ */
