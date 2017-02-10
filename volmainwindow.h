#ifndef VOLMAINWINDOW_H
#define VOLMAINWINDOW_H

#include <QMainWindow>
#include <memory>

class VolMainWindow_internal;

class VolMainWindow : public QMainWindow
{
    Q_OBJECT
    
    std::unique_ptr<VolMainWindow_internal> stuff;
public:
    explicit VolMainWindow(QWidget *parent = 0);
    virtual ~VolMainWindow();
    
signals:
    
public slots:
};

#endif // VOLMAINWINDOW_H
