#define _CRT_SECURE_NO_WARNINGS
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QTextBrowser>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QInputDialog>
#include <QCloseEvent>

#include "code/automaton/automaton.h"
#include "code/lexer/lexer.h"
#include "code/parser/parser.h"
#include "code/semantic/semantic.h"

class TheoryGUI : public QMainWindow {
    Q_OBJECT

private:
    // Main widgets
    QTabWidget *tabWidget;
    QTextEdit *outputArea;
    QStatusBar *statusBar;
    
    // Automaton tab widgets
    QTableWidget *automatonTable;
    QLineEdit *automatonIdInput;
    QLineEdit *alphabetInput;
    QLineEdit *statesInput;
    QLineEdit *initialStateInput;
    QLineEdit *transitionsInput;
    QLineEdit *finalStatesInput;
    QLineEdit *testInput;
    QTextEdit *automatonOutput;
    
    // Program analysis tab widgets
    QTextEdit *programInput;
    QTextBrowser *tokenOutput;
    QTextBrowser *parseTreeOutput;
    QTextBrowser *semanticOutput;
    QTextBrowser *codeGenOutput;

public:
    TheoryGUI(QWidget *parent = nullptr) : QMainWindow(parent) {
        initializeAutomata();
        setupUI();
        setupMenus();
        connectSignals();
        updateAutomatonTable();
    }

private:
    void initializeAutomata() {
        automata = new automat[automataCapacity]();
    }

    void setupUI() {
        setWindowTitle("Theory of Computation - GUI");
        setGeometry(100, 100, 1200, 800);
        
        // Create central widget with main layout
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Create tab widget
        tabWidget = new QTabWidget();
        mainLayout->addWidget(tabWidget);
        
        // Create tabs
        setupAutomatonTab();
        setupProgramAnalysisTab();
        
        // Status bar
        statusBar = new QStatusBar();
        setStatusBar(statusBar);
        statusBar->showMessage("Ready");
    }

    void setupAutomatonTab() {
        QWidget *automatonTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(automatonTab);
        
        // Create splitter for resizable sections
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        layout->addWidget(splitter);
        
        // Left side - Automaton list and input
        QWidget *leftWidget = new QWidget();
        QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
        
        // Automaton table
        QGroupBox *tableGroup = new QGroupBox("Automaton List");
        QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
        automatonTable = new QTableWidget();
        automatonTable->setColumnCount(4);
        QStringList headers = {"ID", "States", "Alphabet", "Transitions"};
        automatonTable->setHorizontalHeaderLabels(headers);
        automatonTable->horizontalHeader()->setStretchLastSection(true);
        tableLayout->addWidget(automatonTable);
        leftLayout->addWidget(tableGroup);
        
        // Automaton input form
        QGroupBox *inputGroup = new QGroupBox("Add/Edit Automaton");
        QGridLayout *inputLayout = new QGridLayout(inputGroup);
        
        inputLayout->addWidget(new QLabel("ID:"), 0, 0);
        automatonIdInput = new QLineEdit();
        inputLayout->addWidget(automatonIdInput, 0, 1);
        
        inputLayout->addWidget(new QLabel("Alphabet:"), 1, 0);
        alphabetInput = new QLineEdit();
        alphabetInput->setPlaceholderText("e.g., ab");
        inputLayout->addWidget(alphabetInput, 1, 1);
        
        inputLayout->addWidget(new QLabel("States:"), 2, 0);
        statesInput = new QLineEdit();
        statesInput->setPlaceholderText("e.g., q0,q1,q2");
        inputLayout->addWidget(statesInput, 2, 1);
        
        inputLayout->addWidget(new QLabel("Initial State:"), 3, 0);
        initialStateInput = new QLineEdit();
        initialStateInput->setPlaceholderText("e.g., q0");
        inputLayout->addWidget(initialStateInput, 3, 1);
        
        inputLayout->addWidget(new QLabel("Transitions:"), 4, 0);
        transitionsInput = new QLineEdit();
        transitionsInput->setPlaceholderText("e.g., q0,a,q1;q1,b,q2");
        inputLayout->addWidget(transitionsInput, 4, 1);
        
        inputLayout->addWidget(new QLabel("Final States:"), 5, 0);
        finalStatesInput = new QLineEdit();
        finalStatesInput->setPlaceholderText("e.g., q2");
        inputLayout->addWidget(finalStatesInput, 5, 1);
        
        leftLayout->addWidget(inputGroup);
        
        // Automaton control buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *addButton = new QPushButton("Add Automaton");
        QPushButton *deleteButton = new QPushButton("Delete");
        QPushButton *searchButton = new QPushButton("Search");
        QPushButton *clearButton = new QPushButton("Clear Form");
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(searchButton);
        buttonLayout->addWidget(clearButton);
        leftLayout->addLayout(buttonLayout);
        
        // Test automaton section
        QGroupBox *testGroup = new QGroupBox("Test Automaton");
        QVBoxLayout *testLayout = new QVBoxLayout(testGroup);
        
        QHBoxLayout *testInputLayout = new QHBoxLayout();
        testInputLayout->addWidget(new QLabel("Input String:"));
        testInput = new QLineEdit();
        testInputLayout->addWidget(testInput);
        QPushButton *testButton = new QPushButton("Test");
        testInputLayout->addWidget(testButton);
        testLayout->addLayout(testInputLayout);
        
        automatonOutput = new QTextEdit();
        automatonOutput->setMaximumHeight(150);
        testLayout->addWidget(automatonOutput);
        
        leftLayout->addWidget(testGroup);
        leftLayout->addStretch();
        
        // Right side - Output area
        QWidget *rightWidget = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
        
        QLabel *outputLabel = new QLabel("Output:");
        outputLabel->setStyleSheet("font-weight: bold;");
        rightLayout->addWidget(outputLabel);
        
        outputArea = new QTextEdit();
        outputArea->setReadOnly(true);
        rightLayout->addWidget(outputArea);
        
        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setSizes({600, 600});
        
        tabWidget->addTab(automatonTab, "Automaton Management");
        
        // Connect button signals
        connect(addButton, &QPushButton::clicked, this, &TheoryGUI::addAutomatonGUI);
        connect(deleteButton, &QPushButton::clicked, this, &TheoryGUI::deleteAutomatonGUI);
        connect(searchButton, &QPushButton::clicked, this, &TheoryGUI::searchAutomatonGUI);
        connect(clearButton, &QPushButton::clicked, this, &TheoryGUI::clearAutomatonForm);
        connect(testButton, &QPushButton::clicked, this, &TheoryGUI::testAutomatonGUI);
        connect(automatonTable, &QTableWidget::cellClicked, this, &TheoryGUI::loadAutomatonToForm);
    }

    void setupProgramAnalysisTab() {
        QWidget *analysisTab = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(analysisTab);
        
        // Input section
        QGroupBox *inputGroup = new QGroupBox("Program Input (Pseudo-C)");
        QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
        
        programInput = new QTextEdit();
        programInput->setPlaceholderText("Enter your pseudo-C program here...\nExample: int x = 5; if (x > 0) { return x; }");
        programInput->setMaximumHeight(150);
        inputLayout->addWidget(programInput);
        
        QHBoxLayout *analysisButtons = new QHBoxLayout();
        QPushButton *analyzeButton = new QPushButton("Analyze Program");
        QPushButton *clearProgramButton = new QPushButton("Clear");
        QPushButton *loadExampleButton = new QPushButton("Load Example");
        analysisButtons->addWidget(analyzeButton);
        analysisButtons->addWidget(clearProgramButton);
        analysisButtons->addWidget(loadExampleButton);
        analysisButtons->addStretch();
        inputLayout->addLayout(analysisButtons);
        
        layout->addWidget(inputGroup);
        
        // Output tabs
        QTabWidget *outputTabs = new QTabWidget();
        
        // Tokens tab
        tokenOutput = new QTextBrowser();
        outputTabs->addTab(tokenOutput, "Tokens");
        
        // Parse tree tab
        parseTreeOutput = new QTextBrowser();
        outputTabs->addTab(parseTreeOutput, "Parse Tree");
        
        // Semantic analysis tab
        semanticOutput = new QTextBrowser();
        outputTabs->addTab(semanticOutput, "Semantic Analysis");
        
        // Code generation tab
        codeGenOutput = new QTextBrowser();
        outputTabs->addTab(codeGenOutput, "Generated Code (Python)");
        
        layout->addWidget(outputTabs);
        
        tabWidget->addTab(analysisTab, "Program Analysis");
        
        // Connect signals
        connect(analyzeButton, &QPushButton::clicked, this, &TheoryGUI::analyzeProgram);
        connect(clearProgramButton, &QPushButton::clicked, this, &TheoryGUI::clearProgramInput);
        connect(loadExampleButton, &QPushButton::clicked, this, &TheoryGUI::loadExampleProgram);
    }

    void setupMenus() {
        QMenuBar *menuBar = this->menuBar();
        
        // File menu
        QMenu *fileMenu = menuBar->addMenu("File");
        
        QAction *exitAction = new QAction("Exit", this);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(exitAction);
        
        // Help menu
        QMenu *helpMenu = menuBar->addMenu("Help");
        
        QAction *aboutAction = new QAction("About", this);
        connect(aboutAction, &QAction::triggered, this, &TheoryGUI::showAbout);
        helpMenu->addAction(aboutAction);
    }

    void connectSignals() {
        // Additional signal connections can be added here
    }

private slots:
    void addAutomatonGUI() {
        try {
            // Get input values
            QString alphabetStr = alphabetInput->text();
            QString statesStr = statesInput->text();
            QString initialStateStr = initialStateInput->text();
            QString transitionsStr = transitionsInput->text();
            QString finalStatesStr = finalStatesInput->text();
            
            if (alphabetStr.isEmpty() || statesStr.isEmpty() || initialStateStr.isEmpty()) {
                QMessageBox::warning(this, "Input Error", "Please fill in all required fields (Alphabet, States, Initial State)");
                return;
            }
            
            // Add the automaton using GUI-specific function
            addAutomatonFromGUI(alphabetStr, statesStr, initialStateStr, transitionsStr, finalStatesStr);
            
            updateAutomatonTable();
            clearAutomatonForm();
            statusBar->showMessage("Automaton added successfully");
            outputArea->append("Automaton added successfully");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Failed to add automaton: %1").arg(e.what()));
        }
    }

    void deleteAutomatonGUI() {
        bool ok;
        int id = QInputDialog::getInt(this, "Delete Automaton", "Enter Automaton ID:", 1, 1, 1000, 1, &ok);
        
        if (ok) {
            int idx = findAutomatonIndexByID(id);
            if (idx == -1) {
                QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
                return;
            }
            
            // Remove the automaton
            delete[] automata[idx].states;
            delete[] automata[idx].alphabet;
            delete[] automata[idx].delta;
            delete[] automata[idx].stateterminal;
            
            // Shift remaining automata
            for (int i = idx; i < automataCount - 1; i++) {
                automata[i] = automata[i + 1];
            }
            automataCount--;
            
            updateAutomatonTable();
            statusBar->showMessage("Automaton deleted");
            outputArea->append(QString("Automaton %1 deleted").arg(id));
        }
    }

    void searchAutomatonGUI() {
        bool ok;
        int id = QInputDialog::getInt(this, "Search Automaton", "Enter Automaton ID:", 1, 1, 1000, 1, &ok);
        
        if (ok) {
            int idx = findAutomatonIndexByID(id);
            if (idx == -1) {
                QMessageBox::information(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
            } else {
                QString info = QString("Automaton found:\nID: %1\nStates: %2\nAlphabet: %3\nTransitions: %4")
                    .arg(automata[idx].id)
                    .arg(automata[idx].stateCount)
                    .arg(automata[idx].alphabetCount)
                    .arg(automata[idx].transitionCount);
                QMessageBox::information(this, "Automaton Found", info);
                outputArea->append(QString("Found automaton with ID %1").arg(id));
            }
        }
    }

    void testAutomatonGUI() {
        QString inputStr = testInput->text();
        if (inputStr.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter an input string to test");
            return;
        }
        
        bool ok;
        int id = QInputDialog::getInt(this, "Test Automaton", "Enter Automaton ID to test:", 1, 1, 1000, 1, &ok);
        
        if (ok) {
            int idx = findAutomatonIndexByID(id);
            if (idx == -1) {
                QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
                return;
            }
            
            QByteArray ba = inputStr.toLocal8Bit();
            QString errorMessage;
            bool result = simulateGUI(automata[idx], ba.data(), errorMessage);
            
            if (!errorMessage.isEmpty()) {
                QMessageBox::warning(this, "Simulation Error", errorMessage);
                automatonOutput->append(QString("Error: %1").arg(errorMessage));
            } else {
                QString resultStr = result ? "ACCEPTED" : "REJECTED";
                automatonOutput->append(QString("Input '%1': %2").arg(inputStr).arg(resultStr));
                outputArea->append(QString("Tested automaton %1 with input '%2': %3").arg(id).arg(inputStr).arg(resultStr));
            }
        }
    }

    void clearAutomatonForm() {
        automatonIdInput->clear();
        alphabetInput->clear();
        statesInput->clear();
        initialStateInput->clear();
        transitionsInput->clear();
        finalStatesInput->clear();
    }

    void loadAutomatonToForm(int row, int column) {
        if (row >= 0 && row < automatonTable->rowCount()) {
            automatonIdInput->setText(automatonTable->item(row, 0)->text());
            // Load other fields as needed from the automata array
        }
    }

    void updateAutomatonTable() {
        automatonTable->setRowCount(0);
        
        for (int i = 0; i < automataCount; i++) {
            int row = automatonTable->rowCount();
            automatonTable->insertRow(row);
            
            automatonTable->setItem(row, 0, new QTableWidgetItem(QString::number(automata[i].id)));
            
            QString statesStr;
            for (int j = 0; j < automata[i].stateCount; j++) {
                if (j > 0) statesStr += ",";
                statesStr += automata[i].states[j];
            }
            automatonTable->setItem(row, 1, new QTableWidgetItem(statesStr));
            
            QString alphabetStr;
            for (int j = 0; j < automata[i].alphabetCount; j++) {
                if (j > 0) alphabetStr += ",";
                alphabetStr += automata[i].alphabet[j];
            }
            automatonTable->setItem(row, 2, new QTableWidgetItem(alphabetStr));
            
            automatonTable->setItem(row, 3, new QTableWidgetItem(QString::number(automata[i].transitionCount)));
        }
    }

    void analyzeProgram() {
        QString programText = programInput->toPlainText();
        if (programText.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter a program to analyze");
            return;
        }
        
        // Convert QString to char* for existing functions
        QByteArray ba = programText.toLocal8Bit();
        char *inputLine = ba.data();
        
        // Clear previous outputs
        tokenOutput->clear();
        parseTreeOutput->clear();
        semanticOutput->clear();
        codeGenOutput->clear();
        
        try {
            // Lexical analysis
            lexicalAnalysis(inputLine);
            
            // Display tokens
            QString tokenText = "Tokens:\n";
            for (Token *t = tokenListHead; t; t = t->next) {
                tokenText += QString("Token: %1  Lexeme: %2\n").arg(t->type).arg(t->lexeme);
            }
            tokenOutput->setPlainText(tokenText);
            
            // Parsing
            currentToken = tokenListHead;
            TreeNode *parseTree = parseProgram();
            
            if (parseTree && peekType() == T_EOF) {
                QString parseText = "Parse successful.\n\nParse Tree:\n";
                // Note: printTree would need to be modified to return QString
                // For now, we'll show a success message
                parseText += "Parse tree structure would be displayed here.";
                parseTreeOutput->setPlainText(parseText);
                
                bool errorFlag = false;
                semanticCheck(parseTree, errorFlag, false);
                
                if (!errorFlag) {
                    semanticOutput->setPlainText("Semantic check passed.\n\nAll variables declared and used correctly.");
                    
                    QString codeGenText = "Generated Code (to pseudo-Python):\n";
                    // Note: generateCode would need to be modified to return QString
                    codeGenText += "# Python code would be generated here";
                    codeGenOutput->setPlainText(codeGenText);
                } else {
                    semanticOutput->setPlainText("Semantic checks failed.\n\nErrors found in variable declarations or usage.");
                }
            } else {
                parseTreeOutput->setPlainText("Parsing failed.\n\nSyntax errors detected in the input program.");
                semanticOutput->setPlainText("Semantic analysis skipped due to parsing errors.");
                codeGenOutput->setPlainText("Code generation skipped due to parsing errors.");
            }
            
            // Cleanup
            cleanupTokensAndSymbols();
            
            statusBar->showMessage("Program analysis completed");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Analysis Error", QString("Failed to analyze program: %1").arg(e.what()));
        }
    }

    void clearProgramInput() {
        programInput->clear();
        tokenOutput->clear();
        parseTreeOutput->clear();
        semanticOutput->clear();
        codeGenOutput->clear();
    }

    void loadExampleProgram() {
        QString example = "int x = 10;\n"
                         "if (x > 5) {\n"
                         "    int y = x * 2;\n"
                         "    return y;\n"
                         "} else {\n"
                         "    return 0;\n"
                         "}";
        programInput->setPlainText(example);
    }

    void showAbout() {
        QMessageBox::about(this, "About", 
            "Theory of Computation GUI\n\n"
            "A graphical interface for:\n"
            "• Finite Automaton Management\n"
            "• Lexical Analysis\n"
            "• Parsing\n"
            "• Semantic Analysis\n"
            "• Code Generation\n\n"
            "Based on the console application for compiler theory concepts.");
    }

    void cleanupTokensAndSymbols() {
        // Cleanup symbol table
        while (symbolTable) {
            Symbol *tmp = symbolTable;
            symbolTable = symbolTable->next;
            delete[] tmp->name;
            delete tmp;
        }
        
        // Cleanup tokens
        while (tokenListHead) {
            Token *tmp = tokenListHead;
            tokenListHead = tokenListHead->next;
            delete[] tmp->lexeme;
            delete tmp;
        }
        tokenListTail = nullptr;
    }

    void addAutomatonFromGUI(QString alphabetStr, QString statesStr, QString initialStateStr, 
                           QString transitionsStr, QString finalStatesStr) {
        resizeAutomataArray();
        automat A;
        A.id = nextAutomatonID++;

        // Parse alphabet
        QStringList alphabetList = alphabetStr.split("", Qt::SkipEmptyParts);
        A.alphabetCount = alphabetList.size();
        A.alphabet = new char[A.alphabetCount];
        for (int i = 0; i < A.alphabetCount; i++) {
            A.alphabet[i] = alphabetList[i].at(0).toLatin1();
        }

        // Parse states
        QStringList statesList = statesStr.split(",", Qt::SkipEmptyParts);
        A.stateCount = statesList.size();
        A.states = new char[A.stateCount];
        for (int i = 0; i < A.stateCount; i++) {
            A.states[i] = statesList[i].trimmed().at(0).toLatin1();
        }

        // Set initial state
        A.qo = initialStateStr.trimmed().at(0).toLatin1();

        // Parse transitions
        QStringList transitionList = transitionsStr.split(";", Qt::SkipEmptyParts);
        A.transitionCount = transitionList.size();
        A.delta = new transition[A.transitionCount];
        for (int i = 0; i < A.transitionCount; i++) {
            QStringList parts = transitionList[i].trimmed().split(",", Qt::SkipEmptyParts);
            if (parts.size() == 3) {
                A.delta[i].origin = parts[0].trimmed().at(0).toLatin1();
                A.delta[i].label = parts[1].trimmed().at(0).toLatin1();
                A.delta[i].target = parts[2].trimmed().at(0).toLatin1();
            }
        }

        // Parse final states
        QStringList finalStatesList = finalStatesStr.split(",", Qt::SkipEmptyParts);
        A.terminalCount = finalStatesList.size();
        A.stateterminal = new char[A.terminalCount];
        for (int i = 0; i < A.terminalCount; i++) {
            A.stateterminal[i] = finalStatesList[i].trimmed().at(0).toLatin1();
        }

        automata[automataCount++] = A;
    }

    bool simulateGUI(automat &A, const char *input, QString &errorMessage) {
        char current = A.qo;

        for (int i = 0; input[i] != '\0'; i++) {
            char symbol = input[i];
            bool transitionFound = false;

            if (!findInArray(symbol, A.alphabet, A.alphabetCount)) {
                errorMessage = QString("Symbol '%1' not in alphabet").arg(symbol);
                return false;
            }

            for (int t = 0; t < A.transitionCount; t++) {
                if (A.delta[t].origin == current &&
                    A.delta[t].label == symbol) {
                    current = A.delta[t].target;
                    transitionFound = true;
                    break;
                }
            }

            if (!transitionFound) {
                errorMessage = QString("No transition for state '%1' with symbol '%2'").arg(current).arg(symbol);
                return false;
            }
        }

        errorMessage.clear(); // No error
        return findInArray(current, A.stateterminal, A.terminalCount);
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        // Cleanup automata memory
        for (int i = 0; i < automataCount; i++) {
            delete[] automata[i].states;
            delete[] automata[i].alphabet;
            delete[] automata[i].delta;
            delete[] automata[i].stateterminal;
        }
        delete[] automata;
        
        event->accept();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TheoryGUI window;
    window.show();
    
    return app.exec();
}

#include "TheoryGUI_single.moc"
