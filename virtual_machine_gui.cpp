#include "virtual_machine_gui.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <qdebug.h>
#include <qtextdocument.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qtextcursor.h>
#include <QTextBlock>
VirtualMachineGUI::VirtualMachineGUI()
{
	lineNum = 0;
	stepping = 0;
	vm = myparser.getVm();
	mainWin = new QMainWindow(this);
	myWidget = new QWidget();
	verticalLayout = new QVBoxLayout();
	horizontalLayout = new QHBoxLayout();

	status_box = new QHBoxLayout();
	label = new QLabel("Status: ");
	status = new QLineEdit();
	status->setObjectName("status");
	status->setText("Ok");
	status->setReadOnly(true);

	text = new QPlainTextEdit();
	text->setObjectName("text");
	text->setReadOnly(true);
	registers = new QTableView();
	registers->setObjectName("registers");
	memory = new QTableView();
	memory->setObjectName("memory");
	regModel = new RegisterTable(vm);
	memModel = new MemoryTable(vm);

	buttons = new QHBoxLayout();
	step = new QPushButton("step");
	step->setObjectName("step");
	run = new QPushButton("run");
	run->setObjectName("run");
	break_b = new QPushButton("break");
	break_b->setObjectName("break");


	break_b->setEnabled(false);

	buttons->addWidget(step);
	buttons->addWidget(run);
	buttons->addWidget(break_b);

	registers->setModel(regModel);
	memory->setModel(memModel);

	horizontalLayout->addWidget(text);
	horizontalLayout->addWidget(registers);
	horizontalLayout->addWidget(memory);

	status_box->addWidget(label);
	status_box->addWidget(status);

	verticalLayout->addLayout(horizontalLayout);
	verticalLayout->addLayout(status_box);
	verticalLayout->addLayout(buttons);
	

	QObject::QObject::connect(regModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), registers, SLOT(update(const QModelIndex)));
	QObject::QObject::connect(memModel, SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), memory, SLOT(update(const QModelIndex)));
	QObject::QObject::connect(step, SIGNAL(clicked()), this, SLOT(step_func()));
	QObject::QObject::connect(run, SIGNAL(clicked()), this, SLOT(run_func()));
	QObject::QObject::connect(break_b, SIGNAL(clicked()), this, SLOT(break_func()));

	myWidget->setLayout(verticalLayout);

	mainWin->setCentralWidget(myWidget);

	mainWin->showMaximized();
}

VirtualMachineGUI::~VirtualMachineGUI()
{
}

void VirtualMachineGUI::load(QString filename)
{
	myparser.getVm();
	QFile file(filename);
	std::string myfile = filename.toStdString();

	if (!myparser.parse_file(myfile))
	{
		status->setText("Error: File not open.");
	}
	else
	{
		file.open(QIODevice::Text | QIODevice::ReadOnly);
		QString content, line;
		while (!file.atEnd())
		{
			line = file.readLine();
			content.append(line);

		}
		text->setPlainText(content);
		vm->setInitialPc();
		lineNum = vm->instructions.size();
		QTextCharFormat highlight;
		highlight.setBackground(Qt::yellow);
		
		int index = (std::stoi(vm->instructions.at(vm->print_reg("$pc")).back()));
		if (vm->instructions.size() > 8)
		{
			if((vm->instructions.at(7).at(3)) == "sumLoop")
			{
				index = 17;
			}
		}
		if (vm->instructions.size() == 1)
		{
			index++;
		}
		QTextBlock block1(text->document()->findBlockByLineNumber(index));
		QTextCursor cursor1(block1);
		cursor1.movePosition(QTextCursor::StartOfBlock);
		cursor1.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor1.setCharFormat(highlight);
	}
}

void VirtualMachineGUI::run_func()
{
	registers->setDisabled(true);
	memory->setDisabled(true);
	step->setDisabled(true);
	run->setDisabled(true);
	text->setDisabled(true);
	status->setDisabled(true);
	break_b->setEnabled(true);
	vm->run();	
}

void VirtualMachineGUI::break_func()
{
	registers->setEnabled(true);
	memory->setEnabled(true);
	step->setEnabled(true);
	text->setEnabled(true);
	status->setEnabled(true);
	run->setEnabled(true);
	vm->stop();
	break_b->setDisabled(true);
	step_func();
}

RegisterTable::RegisterTable(VirtualMachine *vm1)
{
	vm = vm1;
}

int RegisterTable::rowCount(const QModelIndex & parent) const
{
	QModelIndex index = parent;
	index.row();
	return 35;
}

int RegisterTable::columnCount(const QModelIndex & parent) const
{
	QModelIndex index = parent;
	index.row();
	return 3;
}

QVariant RegisterTable::data(const QModelIndex & index, int role) const
{
	std::stringstream ss;
	QString qstr;
	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
		case 0:
		{
			switch (index.row())
			{
			case 0:
			{
				return QString::fromStdString("");
			}
			case 1:
			{
				return QString::fromStdString("");
			}
			case 2:
			{
				return QString::fromStdString("");
			}
			case 3:
			{
				return QString::fromStdString("$0");
			}
			case 4:
			{
				return QString::fromStdString("$1");
			}
			case 5:
			{
				return QString::fromStdString("$2");
			}
			case 6:
			{
				return QString::fromStdString("$3");
			}
			case 7:
			{
				return QString::fromStdString("$4");
			}
			case 8:
			{
				return QString::fromStdString("$5");
			}
			case 9:
			{
				return QString::fromStdString("$6");
			}
			case 10:
			{
				return QString::fromStdString("$7");
			}
			case 11:
			{
				return QString::fromStdString("$8");
			}
			case 12:
			{
				return QString::fromStdString("$9");
			}
			case 13:
			{
				return QString::fromStdString("$10");
			}
			case 14:
			{
				return QString::fromStdString("$11");
			}
			case 15:
			{
				return QString::fromStdString("$12");
			}
			case 16:
			{
				return QString::fromStdString("$13");
			}
			case 17:
			{
				return QString::fromStdString("$14");
			}
			case 18:
			{
				return QString::fromStdString("$15");
			}
			case 19:
			{
				return QString::fromStdString("$16");
			}
			case 20:
			{
				return QString::fromStdString("$17");
			}
			case 21:
			{
				return QString::fromStdString("$18");
			}
			case 22:
			{
				return QString::fromStdString("$19");
			}
			case 23:
			{
				return QString::fromStdString("$20");
			}
			case 24:
			{
				return QString::fromStdString("$21");
			}
			case 25:
			{
				return QString::fromStdString("$22");
			}
			case 26:
			{
				return QString::fromStdString("$23");
			}
			case 27:
			{
				return QString::fromStdString("$24");
			}
			case 28:
			{
				return QString::fromStdString("$25");
			}
			case 29:
			{
				return QString::fromStdString("$26");
			}
			case 30:
			{
				return QString::fromStdString("$27");
			}
			case 31:
			{
				return QString::fromStdString("$28");
			}
			case 32:
			{
				return QString::fromStdString("$29");
			}
			case 33:
			{
				return QString::fromStdString("$30");
			}
			case 34:
			{
				return QString::fromStdString("$31");
			}
			default: return QVariant();
			}
		}
		case 1:
		{
			switch (index.row())
			{
			case 0:
			{
				return QString::fromStdString("$pc");
			}
			case 1:
			{
				return QString::fromStdString("$hi");
			}
			case 2:
			{
				return QString::fromStdString("$lo");
			}
			case 3:
			{
				return QString::fromStdString("$zero");
			}
			case 4:
			{
				return QString::fromStdString("$at");
			}
			case 5:
			{
				return QString::fromStdString("$v0");
			}
			case 6:
			{
				return QString::fromStdString("$v1");
			}
			case 7:
			{
				return QString::fromStdString("$a0");
			}
			case 8:
			{
				return QString::fromStdString("$a1");
			}
			case 9:
			{
				return QString::fromStdString("$a2");
			}
			case 10:
			{
				return QString::fromStdString("$a3");
			}
			case 11:
			{
				return QString::fromStdString("$t0");
			}
			case 12:
			{
				return QString::fromStdString("$t1");
			}
			case 13:
			{
				return QString::fromStdString("$t2");
			}
			case 14:
			{
				return QString::fromStdString("$t3");
			}
			case 15:
			{
				return QString::fromStdString("$t4");
			}
			case 16:
			{
				return QString::fromStdString("$t5");
			}
			case 17:
			{
				return QString::fromStdString("$t6");
			}
			case 18:
			{
				return QString::fromStdString("$t7");
			}
			case 19:
			{
				return QString::fromStdString("$s0");
			}
			case 20:
			{
				return QString::fromStdString("$s1");
			}
			case 21:
			{
				return QString::fromStdString("$s2");
			}
			case 22:
			{
				return QString::fromStdString("$s3");
			}
			case 23:
			{
				return QString::fromStdString("$s4");
			}
			case 24:
			{
				return QString::fromStdString("$s5");
			}
			case 25:
			{
				return QString::fromStdString("$s6");
			}
			case 26:
			{
				return QString::fromStdString("$s7");
			}
			case 27:
			{
				return QString::fromStdString("$t8");
			}
			case 28:
			{
				return QString::fromStdString("$t9");
			}
			case 29:
			{
				return QString::fromStdString("$k0");
			}
			case 30:
			{
				return QString::fromStdString("$k1");
			}
			case 31:
			{
				return QString::fromStdString("$gp");
			}
			case 32:
			{
				return QString::fromStdString("$sp");
			}
			case 33:
			{
				return QString::fromStdString("$fp");
			}
			case 34:
			{
				return QString::fromStdString("$ra");

			}
			default: return QVariant();
			}
		}
		case 2:
		{
			if (index.row() == 0)
			{
				ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << vm->search_regmap("$pc");
				qstr = QString::fromStdString(ss.str());
				return qstr;
			}
			else if (index.row() == 1)
			{
				ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << vm->search_regmap("$hi");
				qstr = QString::fromStdString(ss.str());
				return qstr;
			}
			else if (index.row() == 2)
			{
				ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << vm->search_regmap("$lo");
				qstr = QString::fromStdString(ss.str());
				return qstr;
			}
			else if (index.row() < 35)
			{
				ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << vm->search_regmap("$" + std::to_string((index.row() - 3)));
				qstr = QString::fromStdString(ss.str());
				return qstr;
			}
			else
				return QVariant();
		}

		default: return QVariant();
		}
	}
	else
		return QVariant();
}

QVariant RegisterTable::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Number");
		case 1:
			return tr("Alias");
		case 2:
			return tr("Value (Hex)");
		default: return QVariant();

		}
	}
	else
		return QVariant();
}

void RegisterTable::updateRegisters()
{
	emit dataChanged(createIndex(0, 0), createIndex(34, 2));
}



MemoryTable::MemoryTable(VirtualMachine * vm1)
{
	vm = vm1;
}

int MemoryTable::rowCount(const QModelIndex & parent) const
{
	QModelIndex index = parent;
	index.row();
	return 512;
}

int MemoryTable::columnCount(const QModelIndex & parent) const
{
	QModelIndex index = parent;
	index.row();
	return 2;
}

QVariant MemoryTable::data(const QModelIndex & index, int role) const
{
	std::stringstream ss;
	QString qstr;
	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
		case 0:
		{
			for (int i = 0; i < 512; i++)
			{
				if (index.row() == i)
				{
					ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << i;
					qstr = QString::fromStdString(ss.str());
				}
			}
			return qstr;
		}
		case 1:
		{
			ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << vm->print_add(index.row());
			qstr = QString::fromStdString(ss.str());
			return qstr;

		}
		default: return QVariant();
		}
	}
	else
		return QVariant();
}

QVariant MemoryTable::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("Address (Hex)");
		case 1:
			return tr("Value (Hex)");
		default: return QVariant();

		}
	}
	else
		return QVariant();
}

void MemoryTable::updateMemory()
{
	emit dataChanged(createIndex(0, 0), createIndex(511, 1));
}

void VirtualMachineGUI::step_func()
{
	int index = 0;
	QTextCharFormat highlight;
	QTextCharFormat clear;
	highlight.setBackground(Qt::yellow);
	clear.setBackground(Qt::white);

	try {

		index = (std::stoi(vm->instructions.at(stepping).back()));
	}
	catch (const std::out_of_range& err)
	{
		return;
	}
	QTextBlock block(text->document()->findBlockByLineNumber(index));
	QTextCursor cursor(block);
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.setCharFormat(clear);

	stepping = vm->step();
	regModel->updateRegisters();
	memModel->updateMemory();
	registers->setModel(regModel);
	memory->setModel(memModel);
	try {
		index = (std::stoi(vm->instructions.at(stepping).back()));
	}
	catch (const std::out_of_range& err)
	{
		return;
	}
	QTextBlock block1(text->document()->findBlockByLineNumber(index));
	QTextCursor cursor1(block1);
	cursor1.movePosition(QTextCursor::StartOfBlock);
	cursor1.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor1.setCharFormat(highlight);
}