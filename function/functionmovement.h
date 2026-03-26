#ifndef FUNCTIONMOVEMENT_H
#define FUNCTIONMOVEMENT_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QScopedPointer>
#include <QMap>
#include "plugin/movement/movementacs.h"

class FunctionMovement : public QObject
{
    Q_OBJECT
public:
    enum Axis{
        X = 2,
        Y = 0,
        Z = 4
    };

public:
    static FunctionMovement* getInstance();
    int absoluteMoveSingle(int axis,double point,QString unit = "mm");
    int relativeMoveSingle(int axis,double point,QString unit = "mm");
    QVector<double> getPosAll();
    double getPosAxis(int axis);
    void startPosAcq();
    void stopPosAcq();

signals:
    void sigError(QString msg);
    void sigInfo(QString msg);
    void sigPosAll(QVector<double> pos);

private slots:
    void onPosTimeOut();

private:
    explicit FunctionMovement(QObject *parent = nullptr);
    static void destroyInstance();
    void initAcs();

private:
    struct moveLimit
    {
        Axis axis;
        double min;
        double max;
    };

private:
    static QMutex mutex;
    static QScopedPointer<FunctionMovement> instance;
    MovementACS acs;
    QTimer posTimer;
    QVector<double> pos;
    QMap<int,moveLimit> limits;

    Q_DISABLE_COPY(FunctionMovement)
};

#endif // FUNCTIONMOVEMENT_H
