
#include <vector>
#include <algorithm>
#include "Menu.h"
#include "MenuOcupacao.h"
#include "MenuHorario.h"
#include "MenuEstudantes.h"


void Menu::start() {
    //! Inicializar menu
    bool exit = false;
    string option;
    while (!exit) {
        std::cout << "====================VISUALIZAR====================\n";
        cout << "1 - OCUPAÇÃO\n";
        cout << "2 - HORÁRIO\n";
        cout << "3 - ESTUDANTES\n";

        cout << "\n4 - ALTERAR HORÁRIOS\n";
        cout << "\n type 'q' to quit\n";
        cout << "==================================================\n";

        getline(cin, option);
        if (option == "1"){
            MenuOcupacao menu1 = MenuOcupacao(gestao);
            exit = menu1.ocupacaoMenu();
        }
        else if (option == "2"){
            MenuHorario menu2 = MenuHorario(gestao);
            exit = menu2.horarioMenu();
        }
        else if (option == "3"){
            MenuEstudantes menu3 = MenuEstudantes(gestao);
            exit = menu3.estudantesMenu();
        }
        else if (option == "4") exit = alterarMenu();
        else if (option == "q") break;
        else cout << "invalid input\n\n";
    }
}

Menu::Menu(const GestaoHor &gestao) : gestao(gestao) {}

bool Menu::alterarMenu() {
    while (true){
        cout << "-> ALTERAR HORÁRIOs\n\n";
        cout << "\t1 - Remover estudante de uma turma/UC\n";
        cout << "\t2 - Adicionar estudante a uma turma/UC\n";
        cout << "\t3 - Alterar turma/UC de um estudante\n";
        cout << "\t4 - Alterar conjunto de turmas/UCs de um estudante\n";
        cout << "\t5 - Reverter última mudança efetuada no sistema\n";

        cout << "\n type 'q' to quit, 'r' to return\n";
        cout << "==================================================\n";

        string option1, numStudent, classStudent, codUCStudent;
        Pedido pedido;
        vector<pair<char,Pedido>> waiting;
        getline(cin, option1);
        if (option1 == "1") {
            cout << "Nºestudante:";
            getline(cin, numStudent);
            cout << "Turma:";
            getline(cin, classStudent);
            cout << "Código UC:";
            getline(cin, codUCStudent);
            pedido.setCode(stoi(numStudent)); pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
            waiting.push_back({'r',pedido});
            gestao.addPedidos(waiting);
        }
        else if (option1 == "2") {
            cout << "Nºestudante:";
            getline(cin, numStudent);
            cout << "Turma:";
            getline(cin, classStudent);
            cout << "Código UC:";
            getline(cin, codUCStudent);
            try {
                pedido.setCode(stoi(numStudent));
            }catch (invalid_argument){
                cout << "invalid input\n\n";
                continue;
            }
            pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
            waiting.push_back({'a',pedido});
            gestao.addPedidos(waiting);
        }
        else if (option1 == "3"){
            cout << "Nºestudante:";
            getline(cin, numStudent);
            try {
                pedido.setCode(stoi(numStudent));
            } catch (invalid_argument) {
                cout << "invalid input\n\n";
                continue;
            }
            cout << "Turma/UC a remover:\n";
            cout << "\tTurma:";
            getline(cin, classStudent);
            cout << "\tCódigo UC:";
            getline(cin, codUCStudent);
            pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
            waiting.push_back({'r',pedido});
            cout << "Turma/UC a adicionar:\n";
            cout << "\tTurma:";
            getline(cin, classStudent);
            cout << "\tCódigo UC:";
            getline(cin, codUCStudent);
            pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
            waiting.push_back({'a',pedido});
            gestao.addPedidos(waiting);
        }
        else if (option1 == "4"){
            cout << "Nºestudante:";
            getline(cin, numStudent);
            try {
                pedido.setCode(stoi(numStudent));
            } catch (invalid_argument) {
                cout << "invalid input\n\n";
                continue;
            }
            while (true){
                cout << "Turma/UC a remover (Press Enter to continue):\n";
                cout << "\tTurma:";
                getline(cin, classStudent);
                if (classStudent == "") break;
                cout << "\tCódigo UC:";
                getline(cin, codUCStudent);
                pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
                waiting.push_back({'r',pedido});
            }
            cout << "------------------------------------------------------\n";
            while (true){
                cout << "Turma/UC a adicionar (Press Enter to continue):\n";
                cout << "\tTurma:";
                getline(cin, classStudent);
                if (classStudent == "") break;
                cout << "\tCódigo UC:";
                getline(cin, codUCStudent);
                pedido.setCodTurma(classStudent); pedido.setCodUc(codUCStudent);
                waiting.push_back({'a',pedido});
            }
            if (!waiting.empty())
                gestao.addPedidos(waiting);
        }
        else if (option1 == "5") {
            gestao.undoHistory();
        }
        else if (option1 == "q") {
            gestao.processPedidos();
            return true;
        }
        else if (option1 == "r"){
            gestao.processPedidos();
            return false;
        }
        else {
            cout << "invalid input\n\n";
        }
    }
}