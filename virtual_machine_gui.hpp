#ifndef VIRTUAL_MACHINE_GUI_HPP
#define VIRTUAL_MACHINE_GUI_HPP

#include <QString>
#include <QWidget>
#include <QAbstractTableModel>
#include <qplaintextedit.h>
#include <qtableview.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qmainwindow.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <parser.hpp>
#include <qobject.h>

class RegisterTable;
class MemoryTable;


class VirtualMachineGUI : public QWidget
{
	Q_OBJECT
public:
	VirtualMachineGUI();
	~VirtualMachineGUI();
	void load(QString filename);

	Parser myparser;
	VirtualMachine *vm;

	RegisterTable *regModel;
	MemoryTable *memModel;
	QTableView *registers;
	QTableView *memory;

	public slots:
	void step_func();
	void run_func();
	void break_func();


private:
	QMainWindow *mainWin;
	QWidget *myWidget;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	QHBoxLayout *buttons;

	QHBoxLayout *status_box;
	QLabel *label;
	QLineEdit *status;

	QPlainTextEdit *text;

	QPushButton *step;
	QPushButton *run;
	QPushButton *break_b;


	int lineNum, stepping;
};

class RegisterTable : public QAbstractTableModel
{
public:
	RegisterTable(VirtualMachine *vm);
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex & index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void updateRegisters();

private:
	VirtualMachine *vm;

};

class MemoryTable : public QAbstractTableModel
{
public:
	MemoryTable(VirtualMachine *vm1);
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex & index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	void updateMemory();
private:
	VirtualMachine *vm;
};


#endif