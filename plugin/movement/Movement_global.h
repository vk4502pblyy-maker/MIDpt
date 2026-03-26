#ifndef MOVEMENT_GLOBAL_H
#define MOVEMENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MOVEMENT_LIBRARY)
#  define MOVEMENT_EXPORT Q_DECL_EXPORT
#else
#  define MOVEMENT_EXPORT Q_DECL_IMPORT
#endif

#endif // MOVEMENT_GLOBAL_H
