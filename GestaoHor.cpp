#include <iostream>
#include "GestaoHor.h"

GestaoHor::GestaoHor() {}

void GestaoHor::addStudent(Estudante s) {
    students.insert(s);
}

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
                        del = *it_del;
                        auto it_del_name = students_byname.find(del);
                        students.erase(it_del);
                        students_byname.erase(it_del_name);
                        students.insert(backup);
                        students_byname.insert(backup);
                    }
                    break;
                }
            }
            if (stopLoop) break;
        }
        pedidos.pop();
    }
}

const set<Estudante> &GestaoHor::getStudents() const {
    return students;
}

const set<Estudante, GestaoHor::cmp> &GestaoHor::getStudentsByname() const {
    return students_byname;
}


//TODO Em princípio está tudo bem, mas fazer mais testes
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
    UCTurma t = UCTurma(turma, uc);
    auto timetable = aulas.find(t);
    for(Aula a : timetable->getTimetable()){
        t.addAula(a);
    }

    if (classConflict(temp,t)){
        students.insert(temp);
        students_byname.insert(temp);
        failed.push_back(pedidos.front());
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
        return false;
    }

    return true;
}