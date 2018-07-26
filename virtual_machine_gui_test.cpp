#include <QTest>
#include <QPlainTextEdit>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>

#include "virtual_machine_gui.hpp"

#include "test_config.hpp"

class VirtualMachineGUITest : public QObject {
	Q_OBJECT

		private slots:

	void initTestCase();
	void test00();
	void test01();

	// TODO: implement additional tests here

private:

	VirtualMachineGUI widget;
	VirtualMachine *vm;
};

// this section just verifies the object names and API
void VirtualMachineGUITest::initTestCase() {

	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.asm"));

	auto textWidget = widget.findChild<QPlainTextEdit *>("text");
	QVERIFY2(textWidget, "Could not find QPLainText widget");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto statusViewWidget = widget.findChild<QLineEdit *>("status");
	QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step");
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}


QTEST_MAIN(VirtualMachineGUITest)
#include "virtual_machine_gui_test.moc"

void VirtualMachineGUITest::test00()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test20.asm"));
	widget.step_func();
	widget.run_func();
	widget.break_func();
}

void VirtualMachineGUITest::test01()
{
	QModelIndex parent;
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test20.asm"));
	for (int i = 0; i < 514; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			widget.memModel->index(i, j).data(Qt::DisplayRole);
		}
	}
	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			widget.regModel->index(i, j).data(Qt::DisplayRole);
		}
	}
	widget.memModel->columnCount(parent);
	widget.regModel->rowCount(parent);
		
	widget.registers->setModel(widget.regModel);
	widget.memory->setModel(widget.memModel);

}


