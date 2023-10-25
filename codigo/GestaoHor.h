#ifndef PROJECT_AED_GESTAOHOR_H
#define PROJECT_AED_GESTAOHOR_H

#include <set>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include "Estudante.h"
#include "Pedido.h"

using namespace std;

/**
 * \brief Classe GestaoHor
 *
 * Classe utilizada para auxiliar na gestão de horários e pedidos.
 */
class GestaoHor{
public:
    /**
     * \brief Default constructor da classe GestaoHor.
     * Complexidade O(1).
     */
    GestaoHor();

    /**
     * Struct utilizado para comparar dois estudantes pelo nome.
     */
    struct cmp {
        bool operator()(const Estudante lhs, const Estudante rhs) const{
            return lhs.getName() < rhs.getName();
        }
    };

    /**
     * Recolher os dados dos estudantes a partir de um ficheiro. Complexidade O(n^2).
     *
     * @param filename1 Nome do ficheiro com os dados dos estudantes.
     * @param filename2 Nome do ficheiro com os dados das aulas.
     */
    void getDataStudent(string filename1, string filename2);

    /**
     * Recolher os dados das aulas a partir de um ficheiro. Complexidade O(n^2).
     *
     * @param filename Nome do ficheiro com os dados das aulas.
     */
    void getDataAula(string filename);

    /**
     * \brief Accessor do atributo "students".
     *
     * Complexidade O(1).
     *
     * @return Set de estudantes.
     */
    const set<Estudante> &getStudents() const;

    /**
     * \brief Accessor do atributo "studentss_byname".
     *
     * Complexidade O(1).
     *
     * @return Set de estudantes ordenado por ordem alfabética.
     */
    const set<Estudante, cmp> &getStudentsByname() const;

    /**
     * \brief Accessor do atributo "aulas".
     *
     * Complexidade O(1).
     *
     * @return Set de turmas de uma determinada UC.
     */
    const set<UCTurma> &getAulas() const;

    /**
     * Determina a ocupação das turmas de uma determinada UC. Complexidade O(n^2).
     *
     * @param uc Código da UC.
     * @return Retorna um vetor de pares constituídos pela ocupação de uma turma e o seu código.
     */
    vector<pair<int,string>> ocupacaoTurmasUC(string uc);

    /**
     * Adiciona um pedido à queue do atributo "pedidos". Complexidade O(1).
     *
     * @param pedido Vetor de pedidos a adicionar.
     */
    void addPedidos(vector<pair<char,Pedido>> pedido);

    /**
     * Processa os pedidos na queue.
     */
    void processPedidos();

    /**
     * Reverte a última mudança efetuada no sistema.
     */
    void undoHistory();

    /**
     * Verifica se há um conflito no horário de um estudante se for adicionada uma determinada turma de uma UC. Complexidade O(n^3).
     *
     * @param estudante Estudante com o horário a verificar.
     * @param new_uct Turma de uma UC a ser adicionada.
     * @return 'true' se houver conflito, 'false' se não.
     */
    bool classConflict(Estudante estudante, UCTurma new_uct);

    /**
     * Remove uma determinada UC do horário de um determinado estudante. Complexidade O(n).
     *
     * @param pedido Pedido com as especificações para realizar a remoção (número do estudante e turma da UC).
     */
    void removeStudentUCClass(Pedido pedido);

    /**
     * Adiciona uma determinada UC ao horário de um estudante (se possivel). Complexidade O(n).
     *
     * @param pedido Pedido com as especificações para realizar a adição (número do estudante e turma da UC).
     * @return 'true' se a adição for realizada com sucesso, 'false' se não.
     */
    bool addStudentUCClass(Pedido pedido);
private:
    set<UCTurma> aulas;
    set<Estudante> students;
    set<Estudante, cmp>students_byname;
    queue<vector<pair<char,Pedido>>> pedidos;
    list<vector<pair<char,Pedido>>> failed;
    stack<vector<pair<char,Pedido>>> history;
    const static int cap = 30;

};

#endif //PROJECT_AED_GESTHOR_H