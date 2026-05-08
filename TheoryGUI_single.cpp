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
#include <QFile>
#include <QTextStream>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QAction>
#include <QInputDialog>
#include <QCloseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPolygonF>
#include <QLineF>
#include <QPen>
#include <QBrush>
#include <QTreeWidget>
#include <QDateTime>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>

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
    QComboBox *automatonPresetInput;
    QComboBox *alphabetInput;
    QComboBox *statesInput;
    QComboBox *initialStateInput;
    QComboBox *transitionsInput;
    QComboBox *finalStatesInput;
    QComboBox *testInput;
    QTextEdit *automatonOutput;
    QPushButton *convertNfaToDfaButton;
    QPushButton *minimizeDfaButton;
    QGraphicsView *automatonGraphView;
    QGraphicsScene *automatonGraphScene;
    QTableWidget *transitionTable;
    
    // Program analysis tab widgets
    QTextEdit *programInput;
    QTextBrowser *tokenOutput;
    QTextBrowser *parseTreeOutput;
    QTreeWidget *parseTreeWidget;
    QTextBrowser *semanticOutput;
    QTextBrowser *codeGenOutput;
    QTextBrowser *traceOutput;
    QListWidget *historyList;
    QTextBrowser *automatonReportOutput;
    QTableWidget *symbolTableView;
    QTextBrowser *grammarOutput;
    QString lastTokenText;
    QString lastParseText;
    QString lastSemanticText;
    QString lastCodeText;
    bool darkModeEnabled = false;

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
        showMaximized();
        
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
        
        inputLayout->addWidget(new QLabel("Automaton Preset:"), 0, 0);
        automatonPresetInput = new QComboBox();
        automatonPresetInput->addItems(QStringList()
            << "Custom (manual entry)"
            << "DFA Example - Ends with 'ab'"
            << "DFA Example - Even number of 0s"
            << "NFA Example - Contains 'ab'"
            << "epsilon-NFA Example");
        inputLayout->addWidget(automatonPresetInput, 0, 1);

        inputLayout->addWidget(new QLabel("ID:"), 1, 0);
        automatonIdInput = new QLineEdit();
        inputLayout->addWidget(automatonIdInput, 1, 1);
        
        inputLayout->addWidget(new QLabel("Alphabet:"), 2, 0);
        alphabetInput = new QComboBox();
        alphabetInput->setEditable(true);
        alphabetInput->addItems(QStringList() << "ab" << "abe" << "01" << "abc");
        alphabetInput->lineEdit()->setPlaceholderText("e.g., ab");
        inputLayout->addWidget(alphabetInput, 2, 1);
        
        inputLayout->addWidget(new QLabel("States:"), 3, 0);
        statesInput = new QComboBox();
        statesInput->setEditable(true);
        statesInput->addItems(QStringList() << "q0,q1,q2" << "A,B,C" << "0,1,2");
        statesInput->lineEdit()->setPlaceholderText("e.g., q0,q1,q2");
        inputLayout->addWidget(statesInput, 3, 1);
        
        inputLayout->addWidget(new QLabel("Initial State:"), 4, 0);
        initialStateInput = new QComboBox();
        initialStateInput->setEditable(true);
        initialStateInput->addItems(QStringList() << "q0" << "A" << "0");
        initialStateInput->lineEdit()->setPlaceholderText("e.g., q0");
        inputLayout->addWidget(initialStateInput, 4, 1);
        
        inputLayout->addWidget(new QLabel("Transitions:"), 5, 0);
        transitionsInput = new QComboBox();
        transitionsInput->setEditable(true);
        transitionsInput->addItems(QStringList()
            << "q0,a,q1;q1,b,q2"
            << "A,a,B;A,b,A;B,a,B;B,b,C;C,a,C;C,b,C"
            << "0,e,1;0,e,2;1,a,1;2,b,2");
        transitionsInput->lineEdit()->setPlaceholderText("e.g., q0,a,q1;q1,b,q2");
        inputLayout->addWidget(transitionsInput, 5, 1);
        
        inputLayout->addWidget(new QLabel("Final States:"), 6, 0);
        finalStatesInput = new QComboBox();
        finalStatesInput->setEditable(true);
        finalStatesInput->addItems(QStringList() << "q2" << "C" << "1,2");
        finalStatesInput->lineEdit()->setPlaceholderText("e.g., q2");
        inputLayout->addWidget(finalStatesInput, 6, 1);
        
        leftLayout->addWidget(inputGroup);
        
        // Automaton control buttons
        QGridLayout *buttonLayout = new QGridLayout();
        QPushButton *addButton = new QPushButton("Add Automaton");
        QPushButton *deleteButton = new QPushButton("Delete");
        QPushButton *searchButton = new QPushButton("Search");
        QPushButton *clearButton = new QPushButton("Clear Form");
        QPushButton *equivButton = new QPushButton("Check DFA Equivalence");
        buttonLayout->addWidget(addButton, 0, 0);
        buttonLayout->addWidget(deleteButton, 0, 1);
        buttonLayout->addWidget(searchButton, 0, 2);
        buttonLayout->addWidget(clearButton, 1, 0);
        buttonLayout->addWidget(equivButton, 1, 1, 1, 2);
        leftLayout->addLayout(buttonLayout);
        
        // Test automaton section
        QGroupBox *testGroup = new QGroupBox("Test Automaton");
        QVBoxLayout *testLayout = new QVBoxLayout(testGroup);
        
        QGridLayout *testInputLayout = new QGridLayout();
        testInputLayout->addWidget(new QLabel("Input String:"), 0, 0);
        testInput = new QComboBox();
        testInput->setEditable(true);
        testInput->addItems(QStringList() << "ab" << "aaab" << "bbb" << "aabb");
        testInput->lineEdit()->setPlaceholderText("e.g., ab");
        testInputLayout->addWidget(testInput, 0, 1, 1, 3);
        QPushButton *testButton = new QPushButton("Test");
        QPushButton *testTraceButton = new QPushButton("Test With Trace");
        QPushButton *epsilonClosureButton = new QPushButton("epsilon-closure");
        QPushButton *exportGraphButton = new QPushButton("Export Graph PNG");
        QPushButton *exportGraphDotButton = new QPushButton("Export Graph DOT");
        testInputLayout->addWidget(testButton, 0, 4);
        testInputLayout->addWidget(testTraceButton, 1, 0);
        testInputLayout->addWidget(epsilonClosureButton, 1, 1);
        testInputLayout->addWidget(exportGraphButton, 1, 2);
        testInputLayout->addWidget(exportGraphDotButton, 1, 3);
        testLayout->addLayout(testInputLayout);
        
        automatonOutput = new QTextEdit();
        automatonOutput->setMaximumHeight(150);
        testLayout->addWidget(automatonOutput);

        QHBoxLayout *transformButtons = new QHBoxLayout();
        convertNfaToDfaButton = new QPushButton("Convert NFA→DFA");
        minimizeDfaButton = new QPushButton("Minimize DFA");
        transformButtons->addWidget(convertNfaToDfaButton);
        transformButtons->addWidget(minimizeDfaButton);
        testLayout->addLayout(transformButtons);
        
        leftLayout->addWidget(testGroup);
        leftLayout->addStretch();
        
        // Right side - Output area
        QWidget *rightWidget = new QWidget();
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
        
        QGroupBox *graphGroup = new QGroupBox("Automaton Graph");
        QVBoxLayout *graphLayout = new QVBoxLayout(graphGroup);
        automatonGraphScene = new QGraphicsScene(this);
        automatonGraphView = new QGraphicsView(automatonGraphScene);
        automatonGraphView->setMinimumHeight(300);
        graphLayout->addWidget(automatonGraphView);
        rightLayout->addWidget(graphGroup);

        QTabWidget *automatonOutputTabs = new QTabWidget();
        outputArea = new QTextEdit();
        outputArea->setReadOnly(true);
        automatonOutputTabs->addTab(outputArea, "General Output");
        automatonReportOutput = new QTextBrowser();
        automatonOutputTabs->addTab(automatonReportOutput, "Algorithms Report");
        transitionTable = new QTableWidget();
        automatonOutputTabs->addTab(transitionTable, "Transition Table");
        rightLayout->addWidget(automatonOutputTabs);
        
        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setSizes({600, 600});
        
        tabWidget->addTab(automatonTab, "Automaton Management");
        
        // Connect button signals
        connect(addButton, &QPushButton::clicked, this, &TheoryGUI::addAutomatonGUI);
        connect(deleteButton, &QPushButton::clicked, this, &TheoryGUI::deleteAutomatonGUI);
        connect(searchButton, &QPushButton::clicked, this, &TheoryGUI::searchAutomatonGUI);
        connect(clearButton, &QPushButton::clicked, this, &TheoryGUI::clearAutomatonForm);
        connect(equivButton, &QPushButton::clicked, this, &TheoryGUI::checkDfaEquivalenceGUI);
        connect(testButton, &QPushButton::clicked, this, &TheoryGUI::testAutomatonGUI);
        connect(testTraceButton, &QPushButton::clicked, this, &TheoryGUI::testAutomatonTraceGUI);
        connect(epsilonClosureButton, &QPushButton::clicked, this, &TheoryGUI::epsilonClosureGUI);
        connect(exportGraphButton, &QPushButton::clicked, this, &TheoryGUI::exportAutomatonGraphPng);
        connect(exportGraphDotButton, &QPushButton::clicked, this, &TheoryGUI::exportAutomatonGraphDot);
        connect(convertNfaToDfaButton, &QPushButton::clicked, this, &TheoryGUI::convertNfaToDfaGUI);
        connect(minimizeDfaButton, &QPushButton::clicked, this, &TheoryGUI::minimizeDfaGUI);
        connect(automatonTable, &QTableWidget::cellClicked, this, &TheoryGUI::loadAutomatonToForm);
        connect(automatonPresetInput, &QComboBox::currentTextChanged, this, &TheoryGUI::applyAutomatonPreset);
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
        
        QVBoxLayout *analysisButtons = new QVBoxLayout();
        QHBoxLayout *analysisButtonsRow1 = new QHBoxLayout();
        QHBoxLayout *analysisButtonsRow2 = new QHBoxLayout();
        QPushButton *analyzeButton = new QPushButton("Analyze Program");
        QPushButton *lexButton = new QPushButton("1) Lex");
        QPushButton *parseButton = new QPushButton("2) Parse");
        QPushButton *semanticButton = new QPushButton("3) Semantic");
        QPushButton *codegenButton = new QPushButton("4) Codegen");
        QPushButton *clearProgramButton = new QPushButton("Clear");
        QPushButton *loadExampleButton = new QPushButton("Load Example");
        QPushButton *presetErrorButton = new QPushButton("Load Error Demo");
        QPushButton *exportReportButton = new QPushButton("Export Report");
        QPushButton *exportCsvButton = new QPushButton("Export CSV");
        QPushButton *saveParseTreeButton = new QPushButton("Save Parse Tree");
        QPushButton *saveParseTreeJsonButton = new QPushButton("Save Parse Tree JSON");
        QPushButton *saveParseTreeDotButton = new QPushButton("Save Parse Tree DOT");
        QPushButton *saveCodeButton = new QPushButton("Save Generated Code");
        QPushButton *programToAutomatonButton = new QPushButton("Program -> Automaton");
        QPushButton *demoModeButton = new QPushButton("Run Demo Mode");
        QPushButton *exportAllButton = new QPushButton("Export All Artifacts");
        analysisButtonsRow1->addWidget(analyzeButton);
        analysisButtonsRow1->addWidget(lexButton);
        analysisButtonsRow1->addWidget(parseButton);
        analysisButtonsRow1->addWidget(semanticButton);
        analysisButtonsRow1->addWidget(codegenButton);
        analysisButtonsRow1->addWidget(clearProgramButton);
        analysisButtonsRow1->addWidget(loadExampleButton);
        analysisButtonsRow1->addWidget(presetErrorButton);

        analysisButtonsRow2->addWidget(exportReportButton);
        analysisButtonsRow2->addWidget(exportCsvButton);
        analysisButtonsRow2->addWidget(saveParseTreeButton);
        analysisButtonsRow2->addWidget(saveParseTreeJsonButton);
        analysisButtonsRow2->addWidget(saveParseTreeDotButton);
        analysisButtonsRow2->addWidget(saveCodeButton);
        analysisButtonsRow2->addWidget(programToAutomatonButton);
        analysisButtonsRow2->addWidget(demoModeButton);
        analysisButtonsRow2->addWidget(exportAllButton);
        analysisButtonsRow2->addStretch();

        analysisButtons->addLayout(analysisButtonsRow1);
        analysisButtons->addLayout(analysisButtonsRow2);
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

        // Interactive parse tree tab
        parseTreeWidget = new QTreeWidget();
        parseTreeWidget->setHeaderLabel("Interactive Parse Tree");
        outputTabs->addTab(parseTreeWidget, "Parse Tree (Interactive)");
        
        // Semantic analysis tab
        semanticOutput = new QTextBrowser();
        outputTabs->addTab(semanticOutput, "Semantic Analysis");
        
        // Code generation tab
        codeGenOutput = new QTextBrowser();
        outputTabs->addTab(codeGenOutput, "Generated Code (Python)");

        // Parsing trace tab
        traceOutput = new QTextBrowser();
        outputTabs->addTab(traceOutput, "Parsing Trace");

        // History tab
        historyList = new QListWidget();
        outputTabs->addTab(historyList, "Run History");

        // Symbol table tab
        symbolTableView = new QTableWidget();
        symbolTableView->setColumnCount(3);
        symbolTableView->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Scope");
        symbolTableView->horizontalHeader()->setStretchLastSection(true);
        outputTabs->addTab(symbolTableView, "Symbol Table");

        // Grammar tab
        grammarOutput = new QTextBrowser();
        grammarOutput->setPlainText(
            "Grammar (simplified):\n"
            "Program -> StatementList\n"
            "StatementList -> Statement StatementList | eps\n"
            "Statement -> int id (= Expression)? ;\n"
            "          | string id (= Expression)? ;\n"
            "          | id = Expression ;\n"
            "          | if ( Expression ) Statement (else Statement)?\n"
            "          | while ( Expression ) Statement\n"
            "          | return Expression ;\n"
            "          | { StatementList }\n\n"
            "Expression precedence:\n"
            "Equality -> Comparison ( (==|!=) Comparison )*\n"
            "Comparison -> Term ( (<|>|<=|>=) Term )*\n"
            "Term -> Factor ( (+|-) Factor )*\n"
            "Factor -> Primary ( (*|/) Primary )*\n"
            "Primary -> id | int | string_literal | (Expression)\n\n"
            "Ambiguity lesson (course-aligned):\n"
            "Ambiguous form: E -> E + E | E * E | (E) | id\n"
            "Expression '2+3*4' has two trees unless precedence is encoded.\n"
            "Current parser resolves ambiguity with precedence levels:\n"
            "  * and / before + and -, then comparisons, then equality.\n");
        outputTabs->addTab(grammarOutput, "Grammar");
        
        layout->addWidget(outputTabs);
        
        tabWidget->addTab(analysisTab, "Program Analysis");
        
        // Connect signals
        connect(analyzeButton, &QPushButton::clicked, this, &TheoryGUI::analyzeProgram);
        connect(lexButton, &QPushButton::clicked, this, &TheoryGUI::analyzeLexOnly);
        connect(parseButton, &QPushButton::clicked, this, &TheoryGUI::analyzeUntilParse);
        connect(semanticButton, &QPushButton::clicked, this, &TheoryGUI::analyzeUntilSemantic);
        connect(codegenButton, &QPushButton::clicked, this, &TheoryGUI::analyzeProgram);
        connect(clearProgramButton, &QPushButton::clicked, this, &TheoryGUI::clearProgramInput);
        connect(loadExampleButton, &QPushButton::clicked, this, &TheoryGUI::loadExampleProgram);
        connect(presetErrorButton, &QPushButton::clicked, this, &TheoryGUI::loadErrorDemoProgram);
        connect(exportReportButton, &QPushButton::clicked, this, &TheoryGUI::exportAnalysisReport);
        connect(exportCsvButton, &QPushButton::clicked, this, &TheoryGUI::exportCsvResults);
        connect(saveParseTreeButton, &QPushButton::clicked, this, &TheoryGUI::saveParseTreeText);
        connect(saveParseTreeJsonButton, &QPushButton::clicked, this, &TheoryGUI::saveParseTreeJson);
        connect(saveParseTreeDotButton, &QPushButton::clicked, this, &TheoryGUI::saveParseTreeDot);
        connect(saveCodeButton, &QPushButton::clicked, this, &TheoryGUI::saveGeneratedCodeText);
        connect(programToAutomatonButton, &QPushButton::clicked, this, &TheoryGUI::generateProgramAutomatonGUI);
        connect(demoModeButton, &QPushButton::clicked, this, &TheoryGUI::runDemoMode);
        connect(exportAllButton, &QPushButton::clicked, this, &TheoryGUI::exportAllArtifacts);
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
        QMenu *viewMenu = menuBar->addMenu("View");
        
        QAction *aboutAction = new QAction("About", this);
        connect(aboutAction, &QAction::triggered, this, &TheoryGUI::showAbout);
        helpMenu->addAction(aboutAction);

        QAction *toggleThemeAction = new QAction("Toggle Dark Mode", this);
        connect(toggleThemeAction, &QAction::triggered, this, &TheoryGUI::toggleDarkMode);
        viewMenu->addAction(toggleThemeAction);
    }

    void connectSignals() {
        // Additional signal connections can be added here
    }

private slots:
    void analyzeLexOnly() { runAnalysisPipeline(1); }
    void analyzeUntilParse() { runAnalysisPipeline(2); }
    void analyzeUntilSemantic() { runAnalysisPipeline(3); }

    void loadErrorDemoProgram() {
        QString badExample = "int x = 10\nx = y + 1;\nreturn x;";
        programInput->setPlainText(badExample);
    }

    void exportAnalysisReport() {
        QString path = QFileDialog::getSaveFileName(this, "Export Analysis Report", "analysis_report.txt", "Text files (*.txt)");
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", "Could not write report file.");
            return;
        }
        QTextStream out(&f);
        out << "Analysis Report - " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n\n";
        out << "Input Program:\n" << programInput->toPlainText() << "\n\n";
        out << "Tokens:\n" << tokenOutput->toPlainText() << "\n\n";
        out << "Parse:\n" << parseTreeOutput->toPlainText() << "\n\n";
        out << "Semantic:\n" << semanticOutput->toPlainText() << "\n\n";
        out << "Generated Code:\n" << codeGenOutput->toPlainText() << "\n";
        f.close();
        statusBar->showMessage("Analysis report exported");
    }

    QJsonObject parseTreeToJson(TreeNode *node) {
        QJsonObject obj;
        if (!node) return obj;
        obj["symbol"] = QString(node->symbol);
        QJsonArray children;
        for (int i = 0; i < node->childCount; i++) {
            children.append(parseTreeToJson(node->children[i]));
        }
        obj["children"] = children;
        return obj;
    }

    void parseTreeToDot(TreeNode *node, QTextStream &out, int &idCounter, int parentId = -1) {
        if (!node) return;
        int myId = idCounter++;
        QString label = QString(node->symbol).replace("\"", "\\\"");
        out << "  n" << myId << " [label=\"" << label << "\"];\n";
        if (parentId >= 0) {
            out << "  n" << parentId << " -> n" << myId << ";\n";
        }
        for (int i = 0; i < node->childCount; i++) {
            parseTreeToDot(node->children[i], out, idCounter, myId);
        }
    }

    void exportCsvResults() {
        QString path = QFileDialog::getSaveFileName(this, "Export CSV", "analysis_tokens.csv", "CSV files (*.csv)");
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", "Could not write CSV file.");
            return;
        }
        QTextStream out(&f);
        out << "token,lexeme,line,col\n";
        for (Token *t = tokenListHead; t; t = t->next) {
            out << "\"" << tokenTypeName(t->type) << "\",\"" << t->lexeme << "\"," << t->line << "," << t->col << "\n";
        }
        f.close();
        statusBar->showMessage("CSV exported");
    }

    void saveParseTreeText() {
        QString path = QFileDialog::getSaveFileName(this, "Save Parse Tree", "parse_tree.txt", "Text files (*.txt)");
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream out(&f);
        out << parseTreeOutput->toPlainText();
        f.close();
        statusBar->showMessage("Parse tree saved");
    }

    void saveParseTreeJson() {
        QString path = QFileDialog::getSaveFileName(this, "Save Parse Tree JSON", "parse_tree.json", "JSON files (*.json)");
        if (path.isEmpty()) return;
        // Reparse current input to get structured tree.
        QString programText = programInput->toPlainText();
        if (programText.isEmpty()) {
            QMessageBox::warning(this, "No Input", "Please enter program text first.");
            return;
        }
        lexicalAnalysis(programText.toLocal8Bit().constData());
        currentToken = tokenListHead;
        TreeNode *tree = parseProgram();
        if (!tree || peekType() != T_EOF) {
            QMessageBox::warning(this, "Parse Failed", "Cannot export JSON: parse failed.");
            cleanupTokensAndSymbols();
            if (tree) freeTree(tree);
            return;
        }
        QJsonDocument doc(parseTreeToJson(tree));
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", "Could not write JSON file.");
            freeTree(tree);
            cleanupTokensAndSymbols();
            return;
        }
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
        freeTree(tree);
        cleanupTokensAndSymbols();
        statusBar->showMessage("Parse tree JSON saved");
    }

    void saveParseTreeDot() {
        QString path = QFileDialog::getSaveFileName(this, "Save Parse Tree DOT", "parse_tree.dot", "DOT files (*.dot)");
        if (path.isEmpty()) return;
        QString programText = programInput->toPlainText();
        if (programText.isEmpty()) {
            QMessageBox::warning(this, "No Input", "Please enter program text first.");
            return;
        }
        lexicalAnalysis(programText.toLocal8Bit().constData());
        currentToken = tokenListHead;
        TreeNode *tree = parseProgram();
        if (!tree || peekType() != T_EOF) {
            QMessageBox::warning(this, "Parse Failed", "Cannot export DOT: parse failed.");
            cleanupTokensAndSymbols();
            if (tree) freeTree(tree);
            return;
        }
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", "Could not write DOT file.");
            freeTree(tree);
            cleanupTokensAndSymbols();
            return;
        }
        QTextStream out(&f);
        out << "digraph ParseTree {\n";
        out << "  rankdir=TB;\n";
        out << "  node [shape=box, style=rounded];\n";
        int idCounter = 0;
        parseTreeToDot(tree, out, idCounter);
        out << "}\n";
        f.close();
        freeTree(tree);
        cleanupTokensAndSymbols();
        statusBar->showMessage("Parse tree DOT saved");
    }

    void saveGeneratedCodeText() {
        QString path = QFileDialog::getSaveFileName(this, "Save Generated Code", "generated_code.py", "All files (*)");
        if (path.isEmpty()) return;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
        QTextStream out(&f);
        out << codeGenOutput->toPlainText();
        f.close();
        statusBar->showMessage("Generated code saved");
    }

    void toggleDarkMode() {
        darkModeEnabled = !darkModeEnabled;
        if (darkModeEnabled) {
            qApp->setStyleSheet(
                "QWidget { background-color: #1e1e1e; color: #e0e0e0; }"
                "QTextEdit, QTextBrowser, QLineEdit, QTableWidget, QTreeWidget, QGraphicsView { background-color: #2a2a2a; color: #e0e0e0; border: 1px solid #555; }"
                "QPushButton { background-color: #3a3a3a; color: #e0e0e0; border: 1px solid #666; padding: 4px; }"
                "QPushButton:hover { background-color: #4a4a4a; }"
                "QTabWidget::pane { border: 1px solid #555; }"
                "QHeaderView::section { background-color: #333; color: #e0e0e0; border: 1px solid #555; }"
            );
            statusBar->showMessage("Dark mode enabled");
        } else {
            qApp->setStyleSheet("");
            statusBar->showMessage("Dark mode disabled");
        }
    }

    void generateProgramAutomatonGUI() {
        QString programText = programInput->toPlainText();
        if (programText.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Enter pseudo-code first.");
            return;
        }
        lexicalAnalysis(programText.toLocal8Bit().constData());
        QVector<Token*> toks;
        for (Token *t = tokenListHead; t; t = t->next) {
            if (t->type != T_EOF) toks.push_back(t);
        }
        if (toks.isEmpty() || toks.size() > 24) {
            QMessageBox::warning(this, "Program->Automaton", "Need 1..24 tokens for this demo conversion.");
            cleanupTokensAndSymbols();
            return;
        }

        resizeAutomataArray();
        automat A;
        A.id = nextAutomatonID++;
        A.stateCount = toks.size() + 1;
        A.states = new char[A.stateCount];
        for (int i = 0; i < A.stateCount; i++) A.states[i] = char('A' + i);
        A.qo = A.states[0];

        QVector<char> alpha;
        QVector<transition> trans;
        auto codeOf = [](TokenType t)->char {
            if (t == T_IDENTIFIER) return 'i';
            if (t == T_INTEGER) return 'n';
            if (t == T_STRING_LITERAL) return 's';
            if (t == T_INT) return 'I';
            if (t == T_STRING) return 'S';
            if (t == T_IF) return 'F';
            if (t == T_ELSE) return 'E';
            if (t == T_WHILE) return 'W';
            if (t == T_RETURN) return 'R';
            if (t == T_ASSIGN) return '=';
            if (t == T_SEMICOLON) return ';';
            return 'x';
        };
        for (int i = 0; i < toks.size(); i++) {
            char c = codeOf(toks[i]->type);
            if (!alpha.contains(c)) alpha.push_back(c);
            transition tr{A.states[i], c, A.states[i+1]};
            trans.push_back(tr);
        }
        A.alphabetCount = alpha.size();
        A.alphabet = new char[A.alphabetCount];
        for (int i = 0; i < A.alphabetCount; i++) A.alphabet[i] = alpha[i];
        A.transitionCount = trans.size();
        A.delta = new transition[A.transitionCount];
        for (int i = 0; i < A.transitionCount; i++) A.delta[i] = trans[i];
        A.terminalCount = 1;
        A.stateterminal = new char[1];
        A.stateterminal[0] = A.states[A.stateCount - 1];

        automata[automataCount++] = A;
        updateAutomatonTable();
        outputArea->append(QString("Program -> Automaton generated as ID %1 (linear token-sequence DFA demo).").arg(A.id));
        cleanupTokensAndSymbols();
        statusBar->showMessage("Program-to-automaton conversion completed");
    }

    void runDemoMode() {
        loadExampleProgram();
        runAnalysisPipeline(4);
        if (automataCount == 0) {
            // Quick built-in DFA demo
            addAutomatonFromGUI("ab", "A,B,C", "A", "A,a,B;B,b,C;C,a,C", "C");
            updateAutomatonTable();
        }
        outputArea->append("Demo mode finished: analysis pipeline executed and automaton data ready.");
        statusBar->showMessage("Demo mode completed");
    }

    void applyAutomatonPreset(const QString &preset) {
        if (preset == "Custom (manual entry)") {
            return;
        }
        if (preset == "DFA Example - Ends with 'ab'") {
            alphabetInput->setEditText("ab");
            statesInput->setEditText("A,B,C");
            initialStateInput->setEditText("A");
            transitionsInput->setEditText("A,a,B;A,b,A;B,a,B;B,b,C;C,a,B;C,b,A");
            finalStatesInput->setEditText("C");
            testInput->setEditText("aab");
            return;
        }
        if (preset == "DFA Example - Even number of 0s") {
            alphabetInput->setEditText("01");
            statesInput->setEditText("E,O");
            initialStateInput->setEditText("E");
            transitionsInput->setEditText("E,0,O;E,1,E;O,0,E;O,1,O");
            finalStatesInput->setEditText("E");
            testInput->setEditText("10100");
            return;
        }
        if (preset == "NFA Example - Contains 'ab'") {
            alphabetInput->setEditText("ab");
            statesInput->setEditText("S,A,F");
            initialStateInput->setEditText("S");
            transitionsInput->setEditText("S,a,S;S,b,S;S,a,A;A,b,F;F,a,F;F,b,F");
            finalStatesInput->setEditText("F");
            testInput->setEditText("baab");
            return;
        }
        if (preset == "epsilon-NFA Example") {
            alphabetInput->setEditText("abe");
            statesInput->setEditText("0,1,2");
            initialStateInput->setEditText("0");
            transitionsInput->setEditText("0,e,1;0,e,2;1,a,1;2,b,2");
            finalStatesInput->setEditText("1,2");
            testInput->setEditText("aaa");
        }
    }

    void addAutomatonGUI() {
        try {
            // Get input values
            QString alphabetStr = alphabetInput->currentText();
            QString statesStr = statesInput->currentText();
            QString initialStateStr = initialStateInput->currentText();
            QString transitionsStr = transitionsInput->currentText();
            QString finalStatesStr = finalStatesInput->currentText();
            
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
            if (automataCount > 0) {
                const automat &A = automata[automataCount - 1];
                outputArea->append(QString("Determinism check: %1").arg(isDFA(A) ? "DFA (deterministic)" : "NFA (non-deterministic)"));
            }
            
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
        QString inputStr = testInput->currentText();
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

            std::string trace;
            std::string errorMessage;
            bool result = simulateWithTrace(automata[idx], inputStr.toStdString(), trace, errorMessage);
            QString resultStr = result ? "ACCEPTED" : "REJECTED";

            automatonOutput->append(QString("Input '%1': %2").arg(inputStr).arg(resultStr));
            automatonReportOutput->setPlainText(QString::fromStdString(trace));
            outputArea->append(QString("Tested automaton %1 with input '%2': %3").arg(id).arg(inputStr).arg(resultStr));
            updateTransitionTable(idx);

            if (!errorMessage.empty()) {
                QMessageBox::warning(this, "Simulation Error", QString::fromStdString(errorMessage));
            }
        }
    }

    void testAutomatonTraceGUI() {
        QString inputStr = testInput->currentText();
        if (inputStr.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter an input string to test");
            return;
        }
        bool ok;
        int id = QInputDialog::getInt(this, "Test Automaton With Trace", "Enter Automaton ID to test:", 1, 1, 1000000, 1, &ok);
        if (!ok) return;
        int idx = findAutomatonIndexByID(id);
        if (idx == -1) {
            QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
            return;
        }
        std::string trace;
        std::string err;
        bool accepted = simulateWithTrace(automata[idx], inputStr.toStdString(), trace, err);
        automatonReportOutput->setPlainText(QString::fromStdString(trace));
        outputArea->append(QString("Trace test for automaton %1: %2").arg(id).arg(accepted ? "ACCEPTED" : "REJECTED"));
        updateTransitionTable(idx);
    }

    void epsilonClosureGUI() {
        bool okId, okStates;
        int id = QInputDialog::getInt(this, "epsilon-closure", "Automaton ID:", 1, 1, 1000000, 1, &okId);
        if (!okId) return;
        int idx = findAutomatonIndexByID(id);
        if (idx == -1) {
            QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
            return;
        }
        QString states = QInputDialog::getText(this, "epsilon-closure", "Enter state set (comma-separated):", QLineEdit::Normal, QString(automata[idx].qo), &okStates);
        if (!okStates || states.isEmpty()) return;
        std::string report = epsilonClosureReport(automata[idx], states.toStdString());
        automatonReportOutput->setPlainText(QString::fromStdString(report));
        outputArea->append(QString::fromStdString(report));
    }

    void exportAutomatonGraphPng() {
        if (!automatonGraphScene) return;
        QString path = QFileDialog::getSaveFileName(this, "Export Automaton Graph", "automaton_graph.png", "PNG image (*.png)");
        if (path.isEmpty()) return;
        QRectF r = automatonGraphScene->sceneRect();
        if (r.isEmpty()) r = QRectF(0, 0, 800, 600);
        QImage img(r.size().toSize(), QImage::Format_ARGB32);
        img.fill(Qt::white);
        QPainter p(&img);
        automatonGraphScene->render(&p);
        p.end();
        if (!img.save(path)) {
            QMessageBox::warning(this, "Export Failed", "Could not save PNG.");
            return;
        }
        statusBar->showMessage("Automaton graph exported");
    }

    void exportAutomatonGraphDot() {
        bool ok;
        int id = QInputDialog::getInt(this, "Export Graph DOT", "Automaton ID:", 1, 1, 1000000, 1, &ok);
        if (!ok) return;
        int idx = findAutomatonIndexByID(id);
        if (idx < 0) {
            QMessageBox::warning(this, "Not Found", "Automaton ID not found.");
            return;
        }
        QString path = QFileDialog::getSaveFileName(this, "Export Automaton DOT", "automaton.dot", "DOT files (*.dot)");
        if (path.isEmpty()) return;
        const automat &A = automata[idx];
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Export Failed", "Could not write DOT file.");
            return;
        }
        QTextStream out(&f);
        out << "digraph Automaton {\n";
        out << "  rankdir=LR;\n";
        out << "  node [shape=circle];\n";
        out << "  start [shape=point];\n";
        out << "  start -> \"" << A.qo << "\";\n";
        for (int i = 0; i < A.terminalCount; i++) {
            out << "  \"" << A.stateterminal[i] << "\" [shape=doublecircle];\n";
        }
        for (int i = 0; i < A.transitionCount; i++) {
            out << "  \"" << A.delta[i].origin << "\" -> \"" << A.delta[i].target
                << "\" [label=\"" << A.delta[i].label << "\"];\n";
        }
        out << "}\n";
        f.close();
        statusBar->showMessage("Automaton DOT exported");
    }

    void exportAllArtifacts() {
        QString dir = QFileDialog::getExistingDirectory(this, "Export All Artifacts");
        if (dir.isEmpty()) return;

        // 1) Main report
        {
            QFile f(dir + "/analysis_report.txt");
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&f);
                out << "Analysis Report - " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n\n";
                out << "Input Program:\n" << programInput->toPlainText() << "\n\n";
                out << "Tokens:\n" << tokenOutput->toPlainText() << "\n\n";
                out << "Parse:\n" << parseTreeOutput->toPlainText() << "\n\n";
                out << "Semantic:\n" << semanticOutput->toPlainText() << "\n\n";
                out << "Generated Code:\n" << codeGenOutput->toPlainText() << "\n\n";
                out << "Parsing Trace:\n" << traceOutput->toPlainText() << "\n";
                f.close();
            }
        }

        // 2) CSV tokens
        {
            QFile f(dir + "/analysis_tokens.csv");
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&f);
                out << "token,lexeme,line,col\n";
                for (Token *t = tokenListHead; t; t = t->next) {
                    out << "\"" << tokenTypeName(t->type) << "\",\"" << t->lexeme << "\"," << t->line << "," << t->col << "\n";
                }
                f.close();
            }
        }

        // 3) Parse tree text
        {
            QFile f(dir + "/parse_tree.txt");
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&f);
                out << parseTreeOutput->toPlainText();
                f.close();
            }
        }

        // 4) Generated code
        {
            QFile f(dir + "/generated_code.py");
            if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&f);
                out << codeGenOutput->toPlainText();
                f.close();
            }
        }

        // 5) Parse tree JSON + DOT by reparsing input
        {
            QString programText = programInput->toPlainText();
            if (!programText.isEmpty()) {
                lexicalAnalysis(programText.toLocal8Bit().constData());
                currentToken = tokenListHead;
                TreeNode *tree = parseProgram();
                if (tree && peekType() == T_EOF) {
                    QFile fj(dir + "/parse_tree.json");
                    if (fj.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QJsonDocument doc(parseTreeToJson(tree));
                        fj.write(doc.toJson(QJsonDocument::Indented));
                        fj.close();
                    }
                    QFile fd(dir + "/parse_tree.dot");
                    if (fd.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&fd);
                        out << "digraph ParseTree {\n  rankdir=TB;\n  node [shape=box, style=rounded];\n";
                        int idCounter = 0;
                        parseTreeToDot(tree, out, idCounter);
                        out << "}\n";
                        fd.close();
                    }
                    freeTree(tree);
                } else if (tree) {
                    freeTree(tree);
                }
                cleanupTokensAndSymbols();
            }
        }

        // 6) Graph PNG from current scene
        if (automatonGraphScene) {
            QRectF r = automatonGraphScene->sceneRect();
            if (r.isEmpty()) r = QRectF(0, 0, 800, 600);
            QImage img(r.size().toSize(), QImage::Format_ARGB32);
            img.fill(Qt::white);
            QPainter p(&img);
            automatonGraphScene->render(&p);
            p.end();
            img.save(dir + "/automaton_graph.png");
        }

        statusBar->showMessage("All artifacts exported");
    }

    bool acceptsDfaInput(const automat &A, const QString &input) {
        char current = A.qo;
        QByteArray ba = input.toLocal8Bit();
        const char *s = ba.constData();
        for (int i = 0; s[i] != '\0'; i++) {
            char symbol = s[i];
            bool moved = false;
            for (int t = 0; t < A.transitionCount; t++) {
                if (A.delta[t].origin == current && A.delta[t].label == symbol) {
                    current = A.delta[t].target;
                    moved = true;
                    break;
                }
            }
            if (!moved) return false;
        }
        return findInArray(current, A.stateterminal, A.terminalCount);
    }

    void generateAllStrings(const QString &alphabet, int maxLen, QString current, QStringList &out) {
        out << current;
        if (current.size() >= maxLen) return;
        for (const QChar &c : alphabet) {
            generateAllStrings(alphabet, maxLen, current + c, out);
        }
    }

    void checkDfaEquivalenceGUI() {
        bool ok1, ok2;
        int id1 = QInputDialog::getInt(this, "DFA Equivalence", "First DFA ID:", 1, 1, 1000000, 1, &ok1);
        if (!ok1) return;
        int id2 = QInputDialog::getInt(this, "DFA Equivalence", "Second DFA ID:", 1, 1, 1000000, 1, &ok2);
        if (!ok2) return;

        int i1 = findAutomatonIndexByID(id1);
        int i2 = findAutomatonIndexByID(id2);
        if (i1 < 0 || i2 < 0) {
            QMessageBox::warning(this, "Not Found", "One or both automata IDs were not found.");
            return;
        }
        if (!isDFA(automata[i1]) || !isDFA(automata[i2])) {
            QMessageBox::warning(this, "Invalid Input", "Both automata must be DFAs.");
            return;
        }

        std::string report;
        bool equivalent = areDfaEquivalentExact(automata[i1], automata[i2], report);
        outputArea->append(QString("DFA equivalence (exact): IDs %1 vs %2 => %3")
                           .arg(id1).arg(id2).arg(equivalent ? "Equivalent" : "Not equivalent"));
        outputArea->append(QString::fromStdString(report));
    }

    void convertNfaToDfaGUI() {
        bool ok;
        int id = QInputDialog::getInt(this, "Convert NFA→DFA", "Enter Automaton ID (NFA):", 1, 1, 1000000, 1, &ok);
        if (!ok) return;
        int idx = findAutomatonIndexByID(id);
        if (idx == -1) {
            QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
            return;
        }
        string report;
        automat dfa = convertNfaToDfa(automata[idx], report);
        resizeAutomataArray();
        automata[automataCount++] = dfa;
        updateAutomatonTable();
        outputArea->append(QString("Converted automaton %1 (NFA) to new DFA ID %2").arg(id).arg(dfa.id));
        outputArea->append(QString::fromStdString(report));
        automatonReportOutput->setPlainText(QString::fromStdString(report));
        statusBar->showMessage("NFA→DFA conversion completed");
    }

    void minimizeDfaGUI() {
        bool ok;
        int id = QInputDialog::getInt(this, "Minimize DFA", "Enter Automaton ID (DFA):", 1, 1, 1000000, 1, &ok);
        if (!ok) return;
        int idx = findAutomatonIndexByID(id);
        if (idx == -1) {
            QMessageBox::warning(this, "Not Found", QString("Automaton with ID %1 not found").arg(id));
            return;
        }
        automat dfaSource = automata[idx];
        string report;

        if (!isDFA(automata[idx])) {
            auto ans = QMessageBox::question(
                this,
                "NFA Detected",
                "This automaton is not deterministic.\nConvert NFA→DFA first, then minimize?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            );
            if (ans != QMessageBox::Yes) {
                return;
            }
            string convertReport;
            automat converted = convertNfaToDfa(automata[idx], convertReport);
            resizeAutomataArray();
            automata[automataCount++] = converted;
            updateAutomatonTable();
            outputArea->append(QString("Auto-converted NFA %1 to DFA ID %2 before minimization.")
                               .arg(id).arg(converted.id));
            outputArea->append(QString::fromStdString(convertReport));
            automatonReportOutput->setPlainText(QString::fromStdString(convertReport));
            dfaSource = converted;
        }

        automat minDfa = minimizeDfa(dfaSource, report);
        resizeAutomataArray();
        automata[automataCount++] = minDfa;
        updateAutomatonTable();
        outputArea->append(QString("Minimized automaton source to new DFA ID %1").arg(minDfa.id));
        outputArea->append(QString::fromStdString(report));
        automatonReportOutput->setPlainText(QString::fromStdString(report));
        statusBar->showMessage("DFA minimization completed");
    }

    void clearAutomatonForm() {
        automatonIdInput->clear();
        automatonPresetInput->setCurrentIndex(0);
        alphabetInput->setEditText("");
        statesInput->setEditText("");
        initialStateInput->setEditText("");
        transitionsInput->setEditText("");
        finalStatesInput->setEditText("");
        testInput->setEditText("");
    }

    void loadAutomatonToForm(int row, int column) {
        if (row >= 0 && row < automatonTable->rowCount()) {
            automatonIdInput->setText(automatonTable->item(row, 0)->text());
            if (row < automataCount) {
                drawAutomatonGraph(row);
                updateTransitionTable(row);
            }
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
        if (automataCount > 0) {
            drawAutomatonGraph(0);
            updateTransitionTable(0);
        } else if (automatonGraphScene) {
            automatonGraphScene->clear();
            if (transitionTable) {
                transitionTable->clear();
                transitionTable->setRowCount(0);
                transitionTable->setColumnCount(0);
            }
        }
    }

    void updateTransitionTable(int automatonIndex) {
        if (!transitionTable)
            return;
        transitionTable->clear();
        transitionTable->setRowCount(0);
        transitionTable->setColumnCount(0);
        if (automatonIndex < 0 || automatonIndex >= automataCount)
            return;

        const automat &A = automata[automatonIndex];
        transitionTable->setRowCount(A.stateCount);
        transitionTable->setColumnCount(A.alphabetCount + 1);

        QStringList headers;
        headers << "State";
        for (int a = 0; a < A.alphabetCount; a++)
            headers << QString(A.alphabet[a]);
        transitionTable->setHorizontalHeaderLabels(headers);

        for (int s = 0; s < A.stateCount; s++) {
            QString stateLabel = QString(A.states[s]);
            if (A.states[s] == A.qo)
                stateLabel = "-> " + stateLabel;
            if (findInArray(A.states[s], A.stateterminal, A.terminalCount))
                stateLabel += " *";
            transitionTable->setItem(s, 0, new QTableWidgetItem(stateLabel));

            for (int a = 0; a < A.alphabetCount; a++) {
                QStringList targets;
                for (int t = 0; t < A.transitionCount; t++) {
                    if (A.delta[t].origin == A.states[s] && A.delta[t].label == A.alphabet[a])
                        targets << QString(A.delta[t].target);
                }
                transitionTable->setItem(s, a + 1, new QTableWidgetItem(targets.isEmpty() ? "-" : targets.join(",")));
            }
        }
        transitionTable->resizeColumnsToContents();
    }

    void drawAutomatonGraph(int automatonIndex) {
        if (!automatonGraphScene) return;
        automatonGraphScene->clear();
        if (automatonIndex < 0 || automatonIndex >= automataCount) return;

        const automat &A = automata[automatonIndex];
        if (A.stateCount <= 0) return;

        const QRect viewRect = automatonGraphView ? automatonGraphView->viewport()->rect() : QRect(0, 0, 700, 420);
        const double w = std::max(520, viewRect.width() - 20);
        const double h = std::max(360, viewRect.height() - 20);
        const double cx = w / 2.0;
        const double cy = h / 2.0;
        const double radius = std::max(90.0, std::min(w, h) * 0.32);
        const double nodeR = 24.0;
        QVector<QPointF> pos(A.stateCount);

        // For small automata, use layered layout for cleaner visuals.
        if (A.stateCount == 1) {
            pos[0] = QPointF(cx, cy);
        } else if (A.stateCount == 2) {
            pos[0] = QPointF(cx - radius * 0.6, cy);
            pos[1] = QPointF(cx + radius * 0.6, cy);
        } else if (A.stateCount == 3) {
            pos[0] = QPointF(cx, cy - radius * 0.75);
            pos[1] = QPointF(cx + radius * 0.75, cy + radius * 0.55);
            pos[2] = QPointF(cx - radius * 0.75, cy + radius * 0.55);
        } else {
            for (int i = 0; i < A.stateCount; i++) {
                const double ang = (-M_PI / 2.0) + (2.0 * M_PI * i) / A.stateCount;
                pos[i] = QPointF(cx + radius * std::cos(ang), cy + radius * std::sin(ang));
            }
        }

        auto stateIdx = [&](char s) -> int {
            for (int i = 0; i < A.stateCount; i++) if (A.states[i] == s) return i;
            return -1;
        };

        auto drawCurvedArrow = [&](const QPointF &from, const QPointF &to, const QString &label, double bend) {
            QLineF line(from, to);
            if (line.length() < 1.0) return;
            QPointF dir = (line.unitVector().p2() - line.unitVector().p1());
            QPointF start = from + dir * nodeR;
            QPointF end = to - dir * nodeR;

            QPointF mid = (start + end) / 2.0;
            QPointF normal(-(end.y() - start.y()), end.x() - start.x());
            double nlen = std::sqrt(normal.x() * normal.x() + normal.y() * normal.y());
            if (nlen > 0.0) normal /= nlen;
            QPointF ctrl = mid + normal * bend;

            QPainterPath path(start);
            path.quadTo(ctrl, end);
            automatonGraphScene->addPath(path, QPen(QColor(70, 140, 255), 2.2));

            // Arrow head: tangent near end approximated by (end - ctrl)
            QPointF tvec = end - ctrl;
            double tlen = std::sqrt(tvec.x() * tvec.x() + tvec.y() * tvec.y());
            if (tlen > 0.0) tvec /= tlen;
            QPointF base = end - tvec * 10.0;
            QPointF n(-tvec.y(), tvec.x());
            QPolygonF head;
            head << end << (base + n * 4.5) << (base - n * 4.5);
            automatonGraphScene->addPolygon(head, QPen(QColor(70, 140, 255)), QBrush(QColor(70, 140, 255)));

            auto *txt = automatonGraphScene->addText(label);
            txt->setDefaultTextColor(QColor(255, 80, 80));
            txt->setPos(ctrl + QPointF(6, -8));
            QFont f = txt->font();
            f.setBold(true);
            txt->setFont(f);
        };

        // Group transition labels by (origin,target) to reduce clutter.
        std::map<std::pair<int, int>, QStringList> grouped;
        for (int t = 0; t < A.transitionCount; t++) {
            int o = stateIdx(A.delta[t].origin);
            int d = stateIdx(A.delta[t].target);
            if (o < 0 || d < 0) continue;
            grouped[{o, d}].append(QString(A.delta[t].label));
        }

        // Draw transitions first
        for (const auto &kv : grouped) {
            int o = kv.first.first;
            int d = kv.first.second;
            QString label = kv.second.join(",");
            if (o == d) {
                QPointF p = pos[o];
                // Place loop circle neatly on top-center of node.
                const double loopR = nodeR * 0.9;
                QRectF loopRect(p.x() - loopR, p.y() - nodeR - 2.0 * loopR - 8.0, 2 * loopR, 2 * loopR);
                automatonGraphScene->addEllipse(loopRect, QPen(QColor(70, 140, 255), 2.2));
                auto *txt = automatonGraphScene->addText(label);
                txt->setDefaultTextColor(QColor(255, 80, 80));
                txt->setPos(loopRect.right() + 4, loopRect.center().y() - 8);
                QFont f = txt->font();
                f.setBold(true);
                txt->setFont(f);
            } else {
                bool hasReverse = grouped.find({d, o}) != grouped.end();
                double bend = hasReverse ? 24.0 : 0.0;
                if (hasReverse && o > d) bend = -bend;
                drawCurvedArrow(pos[o], pos[d], label, bend);
            }
        }

        // Draw nodes
        for (int i = 0; i < A.stateCount; i++) {
            const bool isFinal = findInArray(A.states[i], A.stateterminal, A.terminalCount);
            QRectF outer(pos[i].x() - nodeR, pos[i].y() - nodeR, 2 * nodeR, 2 * nodeR);
            automatonGraphScene->addEllipse(outer, QPen(QColor(220, 220, 220), 2.2), QBrush(QColor(245, 245, 245)));
            if (isFinal) {
                QRectF inner(pos[i].x() - nodeR + 4, pos[i].y() - nodeR + 4, 2 * (nodeR - 4), 2 * (nodeR - 4));
                automatonGraphScene->addEllipse(inner, QPen(QColor(30, 30, 30), 2), QBrush(Qt::NoBrush));
            }
            auto *txt = automatonGraphScene->addText(QString(A.states[i]));
            txt->setDefaultTextColor(Qt::black);
            QFont f = txt->font();
            f.setBold(true);
            txt->setFont(f);
            txt->setPos(pos[i].x() - 7, pos[i].y() - 12);
        }

        // Initial arrow
        int initIdx = stateIdx(A.qo);
        if (initIdx >= 0) {
            QPointF p = pos[initIdx];
            QPointF from(p.x() - 60, p.y());
            QPointF to(p.x() - nodeR, p.y());
            automatonGraphScene->addLine(QLineF(from, to), QPen(QColor(20, 180, 70), 2.4));
            QPolygonF head;
            head << to << QPointF(to.x() - 8, to.y() - 4) << QPointF(to.x() - 8, to.y() + 4);
            automatonGraphScene->addPolygon(head, QPen(QColor(20, 180, 70)), QBrush(QColor(20, 180, 70)));
        }

        automatonGraphScene->setSceneRect(0, 0, w, h);
    }

    void analyzeProgram() {
        runAnalysisPipeline(4);
    }

    QString formatParserErrorForGui() const {
        if (!hasParserError())
            return "";

        QString raw = QString::fromStdString(parserLastError);
        QString firstLine = raw.section('\n', 0, 0);
        QString hint = raw.contains('\n') ? raw.section('\n', 1) : "";
        QString nearToken = "?";
        QString expected = firstLine;

        int gotPos = firstLine.indexOf("(got '");
        if (gotPos >= 0) {
            int tokenStart = gotPos + 6;
            int tokenEnd = firstLine.indexOf("')", tokenStart);
            if (tokenEnd > tokenStart)
                nearToken = firstLine.mid(tokenStart, tokenEnd - tokenStart);
            expected = firstLine.left(gotPos).trimmed();
        }

        int colonPos = expected.indexOf("): ");
        if (colonPos >= 0)
            expected = expected.mid(colonPos + 3).trimmed();
        expected.replace("expected ", "Expected ");

        QString formatted = "Syntax error near token '" + nearToken + "'\n";
        formatted += expected + "\n";
        if (!hint.isEmpty())
            formatted += hint + "\n";
        formatted += "\nRaw parser message:\n" + raw + "\n";
        return formatted;
    }

    void runAnalysisPipeline(int stage) {
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
        parseTreeWidget->clear();
        semanticOutput->clear();
        codeGenOutput->clear();
        traceOutput->clear();
        symbolTableView->setRowCount(0);
        
        try {
            QString stageName = (stage == 1 ? "Lex" : stage == 2 ? "Parse" : stage == 3 ? "Semantic" : "Codegen");
            historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Start " + stageName);
            // Lexical analysis
            lexicalAnalysis(inputLine);
            
            // Display tokens
            QString tokenText = "Tokens:\n";
            for (Token *t = tokenListHead; t; t = t->next) {
                tokenText += QString("Token: %1  Lexeme: %2  @(%3,%4)\n")
                                 .arg(tokenTypeName(t->type))
                                 .arg(t->lexeme)
                                 .arg(t->line)
                                 .arg(t->col);
            }
            tokenOutput->setPlainText(tokenText);
            if (stage == 1) {
                statusBar->showMessage("Lexical analysis completed");
                historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Completed Lex");
                cleanupTokensAndSymbols();
                return;
            }
            
            // Parsing
            currentToken = tokenListHead;
            TreeNode *parseTree = parseProgram();
            traceOutput->setPlainText(QString::fromStdString(getParserTrace()));
            
            if (parseTree && peekType() == T_EOF) {
                QString parseText = "Parse successful.\n\nParse Tree:\n";
                parseText += printTreeGUI(parseTree, 0);
                parseTreeOutput->setPlainText(parseText);
                populateParseTreeWidget(parseTree, nullptr);
                if (stage == 2) {
                    freeTree(parseTree);
                    cleanupTokensAndSymbols();
                    statusBar->showMessage("Syntax analysis completed");
                    historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Completed Parse");
                    return;
                }
                
                bool errorFlag = false;
                semanticCheck(parseTree, errorFlag, false);
                populateSymbolTableView();
                
                if (!errorFlag) {
                    QString semanticText = "Semantic check passed.\n\nAnalysis Results:\n";
                    semanticText += "✅ Variable declarations: All variables properly declared\n";
                    semanticText += "✅ Variable usage: All variables used before declaration\n";
                    semanticText += "✅ Type checking: All type assignments valid\n";
                    semanticText += "✅ Scope analysis: No scope conflicts detected\n";
                    semanticText += "\n";
                    semanticText += QString::fromStdString(declaredIdentifiersReport()) + "\n";
                    semanticOutput->setPlainText(semanticText);
                    
                    QString codeGenText = "Generated Code (to pseudo-Python):\n";
                    codeGenText += generateCodeGUI(parseTree, 0);
                    codeGenOutput->setPlainText(codeGenText);
                    if (stage == 3) {
                        freeTree(parseTree);
                        cleanupTokensAndSymbols();
                        statusBar->showMessage("Semantic analysis completed");
                        historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Completed Semantic");
                        return;
                    }
                } else {
                    semanticOutput->setPlainText("Semantic analysis failed. See console output for details (undeclared/redeclared/type mismatch).");
                    codeGenOutput->setPlainText("Code generation skipped due to semantic errors.");
                }
                freeTree(parseTree);
            } else {
                QString parseErrorText = "Parsing failed.\n\nSyntax errors detected in input program.\n\n";
                if (hasParserError()) {
                    parseErrorText += "Details:\n";
                    parseErrorText += formatParserErrorForGui() + "\n";
                }
                parseErrorText += "Please check the following:\n";
                parseErrorText += "• Missing semicolons after statements\n";
                parseErrorText += "• Invalid variable names or types\n";
                parseErrorText += "• Unmatched parentheses or braces\n";
                parseErrorText += "• Reserved keywords used as identifiers\n";
                parseTreeOutput->setPlainText(parseErrorText);
                semanticOutput->setPlainText("Semantic analysis skipped due to parsing errors.");
                codeGenOutput->setPlainText("Code generation skipped due to parsing errors.");
                if (parseTree) {
                    freeTree(parseTree);
                }
                historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Parse failed");
            }
            
            // Cleanup
            cleanupTokensAndSymbols();
            
            statusBar->showMessage("Program analysis completed");
            historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Completed Full Analysis");
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Analysis Error", QString("Failed to analyze program: %1").arg(e.what()));
            historyList->addItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " - Analysis exception");
        }
        cleanupTokensAndSymbols();
        
        statusBar->showMessage("Program analysis completed");
        
    }

    void clearProgramInput() {
        programInput->clear();
        tokenOutput->clear();
        parseTreeOutput->clear();
        parseTreeWidget->clear();
        semanticOutput->clear();
        codeGenOutput->clear();
        traceOutput->clear();
        symbolTableView->setRowCount(0);
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
        resetSemanticState();
        
        // Cleanup tokens
        int guard = 0;
        while (tokenListHead && guard < 100000) {
            Token *tmp = tokenListHead;
            tokenListHead = tokenListHead->next;
            delete[] tmp->lexeme;
            delete tmp;
            guard++;
        }
        tokenListHead = nullptr;
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
            QString stateStr = statesList[i].trimmed();
            if (!stateStr.isEmpty()) {
                A.states[i] = stateStr.at(0).toLatin1();
            }
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
                QString originStr = parts[0].trimmed();
                QString labelStr = parts[1].trimmed();
                QString targetStr = parts[2].trimmed();
                if (!originStr.isEmpty() && !labelStr.isEmpty() && !targetStr.isEmpty()) {
                    A.delta[i].origin = originStr.at(0).toLatin1();
                    A.delta[i].label = labelStr.at(0).toLatin1();
                    A.delta[i].target = targetStr.at(0).toLatin1();
                }
            }
        }

        // Parse final states
        QStringList finalStatesList = finalStatesStr.split(",", Qt::SkipEmptyParts);
        A.terminalCount = finalStatesList.size();
        A.stateterminal = new char[A.terminalCount];
        for (int i = 0; i < A.terminalCount; i++) {
            QString finalStateStr = finalStatesList[i].trimmed();
            if (!finalStateStr.isEmpty()) {
                A.stateterminal[i] = finalStateStr.at(0).toLatin1();
            }
        }

        automata[automataCount++] = A;
    }

    QString printTreeGUI(TreeNode *root, int level = 0) {
        if (!root) return "Empty tree";
        
        QString result = QString("  ").repeated(level) + root->symbol + "\n";
        
        for (int i = 0; i < root->childCount; i++) {
            result += printTreeGUI(root->children[i], level + 1);
        }
        
        return result;
    }

    void populateParseTreeWidget(TreeNode *node, QTreeWidgetItem *parent) {
        if (!node) return;
        QTreeWidgetItem *item = nullptr;
        if (parent) {
            item = new QTreeWidgetItem(parent, QStringList(QString(node->symbol)));
        } else {
            item = new QTreeWidgetItem(parseTreeWidget, QStringList(QString(node->symbol)));
            parseTreeWidget->addTopLevelItem(item);
        }
        for (int i = 0; i < node->childCount; i++) {
            populateParseTreeWidget(node->children[i], item);
        }
        if (!parent) parseTreeWidget->expandAll();
    }

    void populateSymbolTableView() {
        symbolTableView->setRowCount(0);
        for (Symbol *s = symbolTable; s; s = s->next) {
            int r = symbolTableView->rowCount();
            symbolTableView->insertRow(r);
            symbolTableView->setItem(r, 0, new QTableWidgetItem(QString(s->name)));
            symbolTableView->setItem(r, 1, new QTableWidgetItem(QString(s->type)));
            symbolTableView->setItem(r, 2, new QTableWidgetItem(QString::number(s->scopeLevel)));
        }
    }

    QString generateCodeGUI(TreeNode *root, int indent = 0) {
        if (!root) return "";
        
        QString result;
        QString indentStr = QString("    ").repeated(indent);
        
        if (strcmp(root->symbol, "Program") == 0) {
            for (int i = 0; i < root->childCount; i++) {
                result += generateCodeGUI(root->children[i], indent);
            }
            return result;
        }
        
        if (strcmp(root->symbol, "StatementList") == 0) {
            for (int i = 0; i < root->childCount; i++) {
                result += generateCodeGUI(root->children[i], indent);
            }
            return result;
        }
        
        if (strcmp(root->symbol, "Statement") == 0) {
            // Handle declarations (int, string)
            if (root->childCount > 0 &&
                (strcmp(root->children[0]->symbol, "int") == 0 ||
                 strcmp(root->children[0]->symbol, "string") == 0)) {
                const char* type = root->children[0]->symbol;
                const char* varName = root->children[1]->symbol;
                result += indentStr + "# Declared: " + type + " " + varName + "\n";
                
                // Check for initialization
                if (root->childCount > 3 && strcmp(root->children[2]->symbol, "=") == 0) {
                    result += indentStr + varName + " = " + generateCodeGUI(root->children[3], 0) + "\n";
                }
                return result;
            }
            
            // Handle if statements
            if (root->childCount > 0 && strcmp(root->children[0]->symbol, "if") == 0) {
                result += indentStr + "if ";
                // Get condition from children[2] (after "if" and "(")
                if (root->childCount > 2) {
                    result += generateCodeGUI(root->children[2], 0);
                }
                result += ":\n";
                // Get then statement from children[3]
                if (root->childCount > 3) {
                    result += generateCodeGUI(root->children[3], indent + 1);
                }
                // Check for else
                for (int i = 4; i < root->childCount; i++) {
                    if (strcmp(root->children[i]->symbol, "else") == 0) {
                        result += indentStr + "else:\n";
                        if (i + 1 < root->childCount) {
                            result += generateCodeGUI(root->children[i + 1], indent + 1);
                        }
                        break;
                    }
                }
                return result;
            }
            
            // Handle while statements
            if (root->childCount > 0 && strcmp(root->children[0]->symbol, "while") == 0) {
                result += indentStr + "while ";
                // Get condition from children[2] (after "while" and "(")
                if (root->childCount > 2) {
                    result += generateCodeGUI(root->children[2], 0);
                }
                result += ":\n";
                // Get body from children[3]
                if (root->childCount > 3) {
                    result += generateCodeGUI(root->children[3], indent + 1);
                }
                return result;
            }
            
            // Handle return statements
            if (root->childCount > 0 && strcmp(root->children[0]->symbol, "return") == 0) {
                result += indentStr + "return";
                if (root->childCount > 1) {
                    result += " " + generateCodeGUI(root->children[1], 0);
                }
                result += "\n";
                return result;
            }
            
            // Handle assignments (identifier = expression;)
            if (root->childCount > 0 && 
                root->children[0]->childCount == 0 && 
                isIdentifierStart(root->children[0]->symbol[0])) {
                if (root->childCount > 1 && strcmp(root->children[1]->symbol, "=") == 0) {
                    result += indentStr + root->children[0]->symbol + " = ";
                    if (root->childCount > 2) {
                        result += generateCodeGUI(root->children[2], 0);
                    }
                    result += "\n";
                    return result;
                }
            }
            
            // Handle blocks { ... }
            if (root->childCount > 0 && strcmp(root->children[0]->symbol, "{") == 0) {
                for (int i = 1; i < root->childCount - 1; i++) {
                    result += generateCodeGUI(root->children[i], indent);
                }
                return result;
            }
            
            return result;  // Empty statement or unknown
        }
        
        // For leaf nodes (identifiers, numbers, operators)
        if (root->childCount == 0) {
            // Skip punctuation tokens in output
            if (strcmp(root->symbol, ";") == 0 || 
                strcmp(root->symbol, "{") == 0 || 
                strcmp(root->symbol, "}") == 0 ||
                strcmp(root->symbol, "(") == 0 ||
                strcmp(root->symbol, ")") == 0) {
                return "";
            }
            return QString(root->symbol);
        }
        
        // For expression nodes (with children), combine them
        for (int i = 0; i < root->childCount; i++) {
            QString childCode = generateCodeGUI(root->children[i], 0);
            if (!childCode.isEmpty()) {
                if (!result.isEmpty() && !childCode.startsWith(" ")) {
                    result += " ";
                }
                result += childCode;
            }
        }
        
        return result;
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
