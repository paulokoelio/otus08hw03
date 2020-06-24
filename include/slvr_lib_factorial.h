/**
\file
\brief Реализация функции llfactorial(n)

Функция вычисляет факториал от n. Знак n не учитывается (отбрасывается).  
*/
#ifndef SLVR_LIB_FACTORIAL_H_
#define SLVR_LIB_FACTORIAL_H_

#include <cmath>
#include <stdexcept>

namespace slvr
{

    namespace lib
    {

        long long llfactorial(const int n) 
        {
            if ( n == 0 ) 
                return 1;
            long long res = 1;
            for (int i = abs(n); i > 0; i--) 
            { 
                long long temp = res;
                res = res * i;  
                if (temp > res) 
                    throw std::overflow_error("llfactorial function result is too big");
            }
            return res;
        }  


    } // namespace lib

} // namespace slvr

#endif /* SLVR_LIB_FACTORIAL_H_ */