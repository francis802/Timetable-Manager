#include <iostream>
#include "GestaoHor.h"

GestaoHor::GestaoHor() {}

void GestaoHor::getDataAula(string filename){
    ifstream in(filename);
    string line;
    getline(in, line);
    bool first = true;
    bool exists;
    UCTurma temp;
    string last_uccode;
    list<UCTurma> classes;
    while(getline(in, line)) {
        istringstream iss(line);
        string uccode, classcode;
        string weekDay, startH, duration, type;
        getline(iss, classcode, ',');
        getline(iss, uccode, ',');
        getline(iss, weekDay, ',');
        getline(iss, startH, ',');
        getline(iss, duration, ',');
        getline(iss, type,'\r');
        Aula aula = Aula(weekDay,stof(startH),stof(duration),type);
        if (first){
            temp = UCTurma(classcode, uccode);
            temp.addAula(aula);
            classes.push_back(temp);
            last_uccode = uccode;
            first = false;
            continue;
        }
        if (last_uccode.compare(uccode) == 0){
            exists = false;
            for (UCTurma& ucclass: classes){
                if(ucclass.getCodTurma().compare(classcode) == 0) {
                    ucclass.addAula(aula);
                    exists = true;
                    break;
                }
            }
            if(!exists){
                temp = UCTurma(classcode, uccode);
                temp.addAula(aula);
                classes.push_back(temp);
            }
        }
        else{
            last_uccode = uccode;
            for (UCTurma ucclass: classes){
                aulas.insert(ucclass);
            }
            classes.clear();
            temp = UCTurma(classcode, uccode);
            temp.addAula(aula);
            classes.push_back(temp);
        }
    }
    classes.push_back(temp);
    for (UCTurma ucclass: classes){
        aulas.insert(ucclass);
    }
}

void GestaoHor::getDataStudent(string filename1, string filename2) {
    getDataAula(filename2);
    ifstream in(filename1);
    string line;
    getline(in, line);
    bool first = true;
    Estudante temp;
    while(getline(in, line)) {
        istringstream iss(line);
        string num, name;
        string uccode, classcode;
        getline(iss, num, ',');
        getline(iss, name, ',');
        getline(iss, uccode, ',');
        getline(iss, classcode, '\r');
        if (first){
            temp = Estudante(stoi(num), name);
            for (UCTurma ucturma: aulas){
                if ((ucturma.getCodTurma().compare(classcode) == 0) && (ucturma.getCodUc().compare(uccode) == 0))
                    temp.addTurma(ucturma);
            }
            first = false;
            continue;
        }

        if (temp.getName().compare(name) != 0){
            students.insert(temp);
            students_byname.insert(temp);
            temp = Estudante(stoi(num), name);
        }
        for (UCTurma ucturma: aulas){
            if ((ucturma.getCodTurma().compare(classcode) == 0) && (ucturma.getCodUc().compare(uccode) == 0)){
                temp.addTurma(ucturma);
            }
        }
    }
    students.insert(temp);
    students.insert(temp);
}

vector<pair<int,string>> GestaoHor::ocupacaoTurmasUC(string uc){
    string basis;
    basis.push_back(uc[6]);
    basis.push_back(uc[7]);
    int aux = stoi(basis);
    if (aux < 10) basis = "1LEIC";
    else if (aux < 20) basis = "2LEIC";
    else basis = "3LEIC";
    vector<pair<int, string>> ocupacao;
    for (int i = 1; i <= 16; i++){
        if (i < 10){
            ocupacao.push_back({0,basis + "0" + to_string(i)});
        } else ocupacao.push_back({0,basis + to_string(i)});
    }

    for (Estudante e: getStudents()){
        for (UCTurma t: e.getTurmas()){
            if (t.getCodUc() == uc){
                string turma;
                turma.push_back(t.getCodTurma()[5]);
                turma.push_back(t.getCodTurma()[6]);
                int num_turma = stoi(turma);
                ocupacao[num_turma - 1].first++;
                break;
            }
        }
    }
    return ocupacao;
}

void GestaoHor::addPedidos(vector<pair<char, Pedido>> pedido) {
    pedidos.push(pedido);
}

void GestaoHor::processPedidos() {
    while (!pedidos.empty()){
        Pedido ready_pedido = pedidos.front()[0].second;
        int stuCode = ready_pedido.getCode();
        Estudante backup = Estudante();
        backup.setCode(stuCode);
        auto it1 = students.find(backup);
        if (it1 == students.end()){
            ofstream out;
            out.open("../log_failed_changes.txt",std::ios_base::app);
            cout << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << stuCode << endl;
            out << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << stuCode << endl;
            cout << "\tMotivo: estudante não encontrado\n\n";
            out << "\tMotivo: estudante não encontrado\n\n\n";
            out.close();
            pedidos.pop();
            continue;
        }
        backup = *it1;
        bool stopLoop = false;

        for (pair<char,Pedido> pedido : pedidos.front()){
            switch (pedido.first) {
                case 'r': {
                    removeStudentUCClass(pedido.second);
                    break;
                }
                case 'a':{
                    if (!addStudentUCClass(pedido.second)){
                        stopLoop = true;
                        Estudante del = Estudante();
                        del.setCode(stuCode);
                        auto it_del = students.find(del);
                        if (it_del == students.end()){
                            students.insert(backup);
                            students_byname.insert(backup);
                        } else {
                            del = *it_del;
                            auto it_del_name = students_byname.find(del);
                            students.erase(it_del);
                            students_byname.erase(it_del_name);
                            students.insert(backup);
                            students_byname.insert(backup);
                        }
                    }
                    break;
                }
            }
            if (stopLoop) break;
        }
        if(!stopLoop){
            history.push(pedidos.front());
        }
        pedidos.pop();
    }
}

void GestaoHor::undoHistory() {
    Pedido ready_pedido = history.top()[0].second;
    int stuCode = ready_pedido.getCode();
    Estudante backup = Estudante();
    backup.setCode(stuCode);
    auto it1 = students.find(backup);
    backup = *it1;
    bool stopLoop = false;

    for (pair<char,Pedido> pedido : history.top()){
        switch (pedido.first) {
            case 'a': {
                removeStudentUCClass(pedido.second);
                break;
            }
            case 'r':{
                if (!addStudentUCClass(pedido.second)){
                    stopLoop = true;
                    Estudante del = Estudante();
                    del.setCode(stuCode);
                    auto it_del = students.find(del);
                    if (it_del == students.end()){
                        students.insert(backup);
                        students_byname.insert(backup);
                    } else {
                        del = *it_del;
                        auto it_del_name = students_byname.find(del);
                        students.erase(it_del);
                        students_byname.erase(it_del_name);
                        students.insert(backup);
                        students_byname.insert(backup);
                    }
                }
                break;
            }
        }
        if (stopLoop) break;
    }
    if(stopLoop){
        ofstream out;
        out.open("../log_failed_changes.txt",std::ios_base::app);
        cout << "Erro: Não foi possível reverter o histórico de mudanças efetuadas no estudante " << stuCode << endl;
        out << "Erro: Não foi possível reverter o histórico de mudanças efetuadas no estudante " << stuCode << endl;
        cout << "Não foram efetuadas mudanças no horário do estudante\n\n";
        out << "Não foram efetuadas mudanças no horário do estudante\n\n\n";
        out.close();
    }
    history.pop();
}

const set<Estudante> &GestaoHor::getStudents() const {
    return students;
}

const set<Estudante, GestaoHor::cmp> &GestaoHor::getStudentsByname() const {
    return students_byname;
}

const set<UCTurma> &GestaoHor::getAulas() const {
    return aulas;
}

bool GestaoHor::classConflict(Estudante estudante, UCTurma new_uct) {
    vector<Aula> new_tppl;
    for (Aula a:new_uct.getTimetable()){
        if (a.getType() == "TP" || a.getType() == "PL") new_tppl.push_back(a);
    }

    for (UCTurma uct: estudante.getTurmas()){
        for (Aula a: uct.getTimetable()){
            if (a.getType() == "TP" || a.getType() == "PL"){
                for (Aula new_aula: new_tppl){
                    if (new_aula.getWeekday() == a.getWeekday()){
                        if (new_aula.getStartHour() >= a.getStartHour() && new_aula.getStartHour()<(a.getStartHour()+a.getDuration()))
                            return true;
                        if ((new_aula.getStartHour() + new_aula.getDuration()) >= a.getStartHour() && (new_aula.getStartHour() + new_aula.getDuration())<=(a.getStartHour()+a.getDuration()))
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

void GestaoHor::removeStudentUCClass(Pedido pedido) {
    int num = pedido.getCode();
    string turma = pedido.getCodTurma();
    string uc = pedido.getCodUc();

    Estudante temp = Estudante();
    temp.setCode(num);
    auto it1 = students.find(temp);
    temp = *it1;
    auto it2 = students_byname.find(temp);
    students.erase(it1);
    students_byname.erase(it2);
    UCTurma t = UCTurma(turma, uc);
    temp.removeTurma(t);
    students.insert(temp);
    students_byname.insert(temp);
}

bool GestaoHor::addStudentUCClass(Pedido pedido) {
    int num = pedido.getCode();
    string turma = pedido.getCodTurma();
    string uc = pedido.getCodUc();

    Estudante temp = Estudante();
    temp.setCode(num);
    auto it1 = students.find(temp);
    temp = *it1;
    auto it2 = students_byname.find(temp);
    students.erase(it1);
    students_byname.erase(it2);

    for (UCTurma u: temp.getTurmas()){
        if (u.getCodUc() == uc){
            ofstream out;
            out.open("../log_failed_changes.txt",std::ios_base::app);
            cout << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
            out << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
            cout << "\tMotivo: Não é possível estar em 2 turmas da mesma UC\n";
            out << "\tMotivo: Não é possível estar em 2 turmas da mesma UC\n";
            cout <<"\tEstudante da UC " << uc << ", na turma [" << u.getCodTurma() << "] quer entrar noutra turma da UC: [" << turma << "]\n";
            out <<"\tEstudante da UC " << uc << ", na turma [" << u.getCodTurma() << "] quer entrar noutra turma da UC: [" << turma << "]\n";
            cout << "Não foram efetuadas mudanças no horário do estudante\n\n";
            out << "Não foram efetuadas mudanças no horário do estudante\n\n\n";
            out.close();
            return false;
        }
    }

    UCTurma t = UCTurma(turma, uc);
    auto timetable = aulas.find(t);
    if (timetable == aulas.end()){
        ofstream out;
        out.open("../log_failed_changes.txt",std::ios_base::app);
        cout << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        out << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        cout << "\tMotivo: " << uc << " | " << turma << " não tem aulas associadas\n";
        out << "\tMotivo: " << uc << " | " << turma << " não tem aulas associadas\n";
        cout << "Não foram efetuadas mudanças no horário do estudante\n\n";
        out << "Não foram efetuadas mudanças no horário do estudante\n\n\n";
        out.close();
        return false;
    }
    for(Aula a : timetable->getTimetable()){
        t.addAula(a);
    }

    if (classConflict(temp,t)){
        students.insert(temp);
        students_byname.insert(temp);
        failed.push_back(pedidos.front());
        ofstream out;
        out.open("../log_failed_changes.txt",std::ios_base::app);
        cout << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        out << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        cout << "\tMotivo: " << uc << " | " << turma << " causa sobreposição de aulas\n";
        out << "\tMotivo: " << uc << " | " << turma << " causa sobreposição de aulas\n";
        cout << "Não foram efetuadas mudanças no horário do estudante\n\n";
        out << "Não foram efetuadas mudanças no horário do estudante\n\n\n";
        out.close();
        return false;
    }

    temp.addTurma(t);
    students.insert(temp);
    students_byname.insert(temp);

    vector<pair<int,string>> ocupacao = ocupacaoTurmasUC(uc);
    string num_turma;
    num_turma.push_back(turma[5]);
    num_turma.push_back(turma[6]);
    int min = cap;
    for (pair<int,string> i : ocupacao){
        if (i.first != 0 && i.first < min)
            min = i.first;
    }
    if (ocupacao[stoi(num_turma) - 1].first > cap || abs(ocupacao[stoi(num_turma) - 1].first - min) >= 4){
        failed.push_back(pedidos.front());
        ofstream out;
        out.open("../log_failed_changes.txt",std::ios_base::app);
        cout << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        out << "Erro: Não é possível efetuar as mudanças desejadas ao estudante " << temp.getCode() << endl;
        cout << "\tMotivo: Desequilibrio/sobrelotação de turmas\n";
        out << "\tMotivo: Desequilibrio/sobrelotação de turmas\n";
        cout << "Não foram efetuadas mudanças no horário do estudante\n\n";
        out << "Não foram efetuadas mudanças no horário do estudante\n\n\n";
        out.close();
        return false;
    }

    return true;
}
