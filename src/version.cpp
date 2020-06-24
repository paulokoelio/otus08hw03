/**
\file
\brief Реализация функции allocator::version() для вывода версии решения ДЗ

Версия решения ДЗ может быть определена в коде программы путем вызова 
функции allocator::version()
*/
#include "version.h"

namespace slvr
{
    namespace allocator
    {

        const char *version()
        {
            return PROJECT_VERSION;
        }

    } // namespace allocator

} // namespace slvr