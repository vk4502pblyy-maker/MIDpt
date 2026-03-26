#ifndef SENSORS_GLOBAL_H
#define SENSORS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SENSORS_LIBRARY)
#  define SENSORS_EXPORT Q_DECL_EXPORT
#else
#  define SENSORS_EXPORT Q_DECL_IMPORT
#endif

#endif // SENSORS_GLOBAL_H
