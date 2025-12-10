#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <cstring>
#include <limits>

using namespace std;

struct Cliente {
    int codigo;
    char nome[100];
    char endereco[200];
    char telefone[20];
};

struct Funcionario {
    int codigo;
    char nome[100];
    char telefone[20];
    char cargo[50];
    double salario;
};

struct Quarto {
    int numero;
    int capacidade;
    double valorDiaria;
    char status[20];
};

struct Estadia {
    int codigo;
    char dataEntrada[11];
    char dataSaida[11];
    int quantidadeDiarias;
    int codigoCliente;
    int numeroQuarto;
};

vector<Cliente> clientes;
vector<Funcionario> funcionarios;
vector<Quarto> quartos;
vector<Estadia> estadias;

const string CLIENTES_FILE = "clientes.dat";
const string FUNCIONARIOS_FILE = "funcionarios.dat";
const string QUARTOS_FILE = "quartos.dat";
const string ESTADIAS_FILE = "estadias.dat";

void carregarDados();
void salvarClientes();
void salvarFuncionarios();
void salvarQuartos();
void salvarEstadias();
bool codigoClienteExiste(int codigo);
bool codigoFuncionarioExiste(int codigo);
bool numeroQuartoExiste(int numero);
bool quartoDisponivel(int numero, const string& entrada, const string& saida);
int calcularDiarias(const string& entrada, const string& saida);
void cadastrarCliente();
void cadastrarFuncionario();
void cadastrarQuarto();
void cadastrarEstadia();
void darBaixaEstadia();
void pesquisarCliente();
void pesquisarFuncionario();
void mostrarEstadiasCliente();
void calcularPontosFidelidade();
int encontrarProximoCodigoCliente();
int encontrarProximoCodigoFuncionario();
int encontrarProximoCodigoEstadia();
void menu();
void limparBuffer();
string paraString(const char* texto);

int main() {
    carregarDados();
    menu();
    
    salvarClientes();
    salvarFuncionarios();
    salvarQuartos();
    salvarEstadias();
    
    return 0;
}

void carregarDados() {
    ifstream arquivoClientes(CLIENTES_FILE.c_str(), ios::binary);
    if (arquivoClientes) {
        Cliente c;
        while (arquivoClientes.read((char*)&c, sizeof(Cliente))) {
            clientes.push_back(c);
        }
        arquivoClientes.close();
    }

    ifstream arquivoFuncionarios(FUNCIONARIOS_FILE.c_str(), ios::binary);
    if (arquivoFuncionarios) {
        Funcionario f;
        while (arquivoFuncionarios.read((char*)&f, sizeof(Funcionario))) {
            funcionarios.push_back(f);
        }
        arquivoFuncionarios.close();
    }

    ifstream arquivoQuartos(QUARTOS_FILE.c_str(), ios::binary);
    if (arquivoQuartos) {
        Quarto q;
        while (arquivoQuartos.read((char*)&q, sizeof(Quarto))) {
            quartos.push_back(q);
        }
        arquivoQuartos.close();
    }

    ifstream arquivoEstadias(ESTADIAS_FILE.c_str(), ios::binary);
    if (arquivoEstadias) {
        Estadia e;
        while (arquivoEstadias.read((char*)&e, sizeof(Estadia))) {
            estadias.push_back(e);
        }
        arquivoEstadias.close();
    }
}

void salvarClientes() {
    ofstream arquivo(CLIENTES_FILE.c_str(), ios::binary);
    if (arquivo) {
        for (size_t i = 0; i < clientes.size(); i++) {
            arquivo.write((const char*)&clientes[i], sizeof(Cliente));
        }
        arquivo.close();
    }
}

void salvarFuncionarios() {
    ofstream arquivo(FUNCIONARIOS_FILE.c_str(), ios::binary);
    if (arquivo) {
        for (size_t i = 0; i < funcionarios.size(); i++) {
            arquivo.write((const char*)&funcionarios[i], sizeof(Funcionario));
        }
        arquivo.close();
    }
}

void salvarQuartos() {
    ofstream arquivo(QUARTOS_FILE.c_str(), ios::binary);
    if (arquivo) {
        for (size_t i = 0; i < quartos.size(); i++) {
            arquivo.write((const char*)&quartos[i], sizeof(Quarto));
        }
        arquivo.close();
    }
}

void salvarEstadias() {
    ofstream arquivo(ESTADIAS_FILE.c_str(), ios::binary);
    if (arquivo) {
        for (size_t i = 0; i < estadias.size(); i++) {
            arquivo.write((const char*)&estadias[i], sizeof(Estadia));
        }
        arquivo.close();
    }
}

bool codigoClienteExiste(int codigo) {
    for (size_t i = 0; i < clientes.size(); i++) {
        if (clientes[i].codigo == codigo) return true;
    }
    return false;
}

bool codigoFuncionarioExiste(int codigo) {
    for (size_t i = 0; i < funcionarios.size(); i++) {
        if (funcionarios[i].codigo == codigo) return true;
    }
    return false;
}

bool numeroQuartoExiste(int numero) {
    for (size_t i = 0; i < quartos.size(); i++) {
        if (quartos[i].numero == numero) return true;
    }
    return false;
}

bool quartoDisponivel(int numero, const string& entrada, const string& saida) {
    for (size_t i = 0; i < quartos.size(); i++) {
        if (quartos[i].numero == numero) {
            if (string(quartos[i].status) != "desocupado") {
                return false;
            }
            
            for (size_t j = 0; j < estadias.size(); j++) {
                if (estadias[j].numeroQuarto == numero) {
                    string eEntrada = estadias[j].dataEntrada;
                    string eSaida = estadias[j].dataSaida;
                    
                    if (!(saida <= eEntrada || entrada >= eSaida)) {
                        return false;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

int calcularDiarias(const string& entrada, const string& saida) {
    int ano1, mes1, dia1, ano2, mes2, dia2;
    
    if (sscanf(entrada.c_str(), "%d-%d-%d", &ano1, &mes1, &dia1) != 3) return 1;
    if (sscanf(saida.c_str(), "%d-%d-%d", &ano2, &mes2, &dia2) != 3) return 1;
    
    struct tm tm1 = {0};
    struct tm tm2 = {0};
    
    tm1.tm_year = ano1 - 1900;
    tm1.tm_mon = mes1 - 1;
    tm1.tm_mday = dia1;
    
    tm2.tm_year = ano2 - 1900;
    tm2.tm_mon = mes2 - 1;
    tm2.tm_mday = dia2;
    
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    
    if (t1 == -1 || t2 == -1) return 1;
    
    double segundos = difftime(t2, t1);
    int dias = (int)(segundos / (60 * 60 * 24));
    
    if (dias <= 0) return 1;
    return dias;
}

int encontrarProximoCodigoCliente() {
    int maior = 0;
    for (size_t i = 0; i < clientes.size(); i++) {
        if (clientes[i].codigo > maior) maior = clientes[i].codigo;
    }
    return maior + 1;
}

int encontrarProximoCodigoFuncionario() {
    int maior = 0;
    for (size_t i = 0; i < funcionarios.size(); i++) {
        if (funcionarios[i].codigo > maior) maior = funcionarios[i].codigo;
    }
    return maior + 1;
}

int encontrarProximoCodigoEstadia() {
    int maior = 0;
    for (size_t i = 0; i < estadias.size(); i++) {
        if (estadias[i].codigo > maior) maior = estadias[i].codigo;
    }
    return maior + 1;
}

void limparBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string paraString(const char* texto) {
    return string(texto);
}

void cadastrarCliente() {
    Cliente novo;
    novo.codigo = encontrarProximoCodigoCliente();
    
    cout << "\n=== CADASTRO DE CLIENTE ===" << endl;
    cout << "Nome: ";
    limparBuffer();
    cin.getline(novo.nome, sizeof(novo.nome));
    
    cout << "Endereco: ";
    cin.getline(novo.endereco, sizeof(novo.endereco));
    
    cout << "Telefone: ";
    cin.getline(novo.telefone, sizeof(novo.telefone));
    
    clientes.push_back(novo);
    salvarClientes();
    cout << "Cliente cadastrado com sucesso! Codigo: " << novo.codigo << endl;
}

void cadastrarFuncionario() {
    Funcionario novo;
    novo.codigo = encontrarProximoCodigoFuncionario();
    
    cout << "\n=== CADASTRO DE FUNCIONARIO ===" << endl;
    cout << "Nome: ";
    limparBuffer();
    cin.getline(novo.nome, sizeof(novo.nome));
    
    cout << "Telefone: ";
    cin.getline(novo.telefone, sizeof(novo.telefone));
    
    cout << "Cargo: ";
    cin.getline(novo.cargo, sizeof(novo.cargo));
    
    cout << "Salario: ";
    while (!(cin >> novo.salario) || novo.salario <= 0) {
        cout << "Salario invalido! Digite novamente: ";
        limparBuffer();
    }
    
    funcionarios.push_back(novo);
    salvarFuncionarios();
    cout << "Funcionario cadastrado com sucesso! Codigo: " << novo.codigo << endl;
}

void cadastrarQuarto() {
    Quarto novo;
    
    cout << "\n=== CADASTRO DE QUARTO ===" << endl;
    cout << "Numero do quarto: ";
    while (!(cin >> novo.numero) || novo.numero <= 0) {
        cout << "Numero invalido! Digite novamente: ";
        limparBuffer();
    }
    
    if (numeroQuartoExiste(novo.numero)) {
        cout << "Quarto ja cadastrado!" << endl;
        return;
    }
    
    cout << "Capacidade: ";
    while (!(cin >> novo.capacidade) || novo.capacidade <= 0) {
        cout << "Capacidade invalida! Digite novamente: ";
        limparBuffer();
    }
    
    cout << "Valor da diaria: ";
    while (!(cin >> novo.valorDiaria) || novo.valorDiaria <= 0) {
        cout << "Valor invalido! Digite novamente: ";
        limparBuffer();
    }
    
    strcpy(novo.status, "desocupado");
    
    quartos.push_back(novo);
    salvarQuartos();
    cout << "Quarto cadastrado com sucesso!" << endl;
}

void cadastrarEstadia() {
    if (clientes.empty()) {
        cout << "Nenhum cliente cadastrado!" << endl;
        return;
    }
    
    if (quartos.empty()) {
        cout << "Nenhum quarto cadastrado!" << endl;
        return;
    }
    
    cout << "\n=== CADASTRO DE ESTADIA ===" << endl;
    
    Estadia nova;
    nova.codigo = encontrarProximoCodigoEstadia();
    
    int codigoCliente;
    cout << "Codigo do cliente: ";
    while (!(cin >> codigoCliente) || codigoCliente <= 0) {
        cout << "Codigo invalido! Digite novamente: ";
        limparBuffer();
    }
    
    bool clienteEncontrado = false;
    for (size_t i = 0; i < clientes.size(); i++) {
        if (clientes[i].codigo == codigoCliente) {
            clienteEncontrado = true;
            break;
        }
    }
    
    if (!clienteEncontrado) {
        cout << "Cliente nao encontrado!" << endl;
        return;
    }
    
    nova.codigoCliente = codigoCliente;
    
    int quantidadeHospedes;
    cout << "Quantidade de hospedes: ";
    while (!(cin >> quantidadeHospedes) || quantidadeHospedes <= 0) {
        cout << "Quantidade invalida! Digite novamente: ";
        limparBuffer();
    }
    
    limparBuffer();
    cout << "Data de entrada (YYYY-MM-DD): ";
    cin.getline(nova.dataEntrada, sizeof(nova.dataEntrada));
    
    cout << "Data de saida (YYYY-MM-DD): ";
    cin.getline(nova.dataSaida, sizeof(nova.dataSaida));
    
    string entradaStr = nova.dataEntrada;
    string saidaStr = nova.dataSaida;
    
    if (saidaStr <= entradaStr) {
        cout << "Data de saida deve ser posterior a data de entrada!" << endl;
        return;
    }
    
    nova.quantidadeDiarias = calcularDiarias(entradaStr, saidaStr);
    
    int quartoEscolhido = -1;
    for (size_t i = 0; i < quartos.size(); i++) {
        if (quartos[i].capacidade >= quantidadeHospedes && 
            quartoDisponivel(quartos[i].numero, entradaStr, saidaStr)) {
            quartoEscolhido = quartos[i].numero;
            strcpy(quartos[i].status, "ocupado");
            break;
        }
    }
    
    if (quartoEscolhido == -1) {
        cout << "Nenhum quarto disponivel para as datas e capacidade informadas!" << endl;
        return;
    }
    
    nova.numeroQuarto = quartoEscolhido;
    
    estadias.push_back(nova);
    salvarQuartos();
    salvarEstadias();
    
    cout << "\nEstadia cadastrada com sucesso!" << endl;
    cout << "Codigo: " << nova.codigo << endl;
    cout << "Quarto: " << nova.numeroQuarto << endl;
    cout << "Diarias: " << nova.quantidadeDiarias << endl;
}

void darBaixaEstadia() {
    if (estadias.empty()) {
        cout << "Nenhuma estadia cadastrada!" << endl;
        return;
    }
    
    cout << "\n=== BAIXA DE ESTADIA ===" << endl;
    int codigoEstadia;
    cout << "Codigo da estadia: ";
    while (!(cin >> codigoEstadia) || codigoEstadia <= 0) {
        cout << "Codigo invalido! Digite novamente: ";
        limparBuffer();
    }
    
    int index = -1;
    int numeroQuarto = -1;
    int quantidadeDiarias = 0;
    
    for (size_t i = 0; i < estadias.size(); i++) {
        if (estadias[i].codigo == codigoEstadia) {
            index = i;
            numeroQuarto = estadias[i].numeroQuarto;
            quantidadeDiarias = estadias[i].quantidadeDiarias;
            break;
        }
    }
    
    if (index == -1) {
        cout << "Estadia nao encontrada!" << endl;
        return;
    }
    
    for (size_t i = 0; i < quartos.size(); i++) {
        if (quartos[i].numero == numeroQuarto) {
            strcpy(quartos[i].status, "desocupado");
            break;
        }
    }
    
    double valorTotal = 0;
    for (size_t i = 0; i < quartos.size(); i++) {
        if (quartos[i].numero == numeroQuarto) {
            valorTotal = quartos[i].valorDiaria * quantidadeDiarias;
            break;
        }
    }
    
    cout << "\n=== RECIBO ===" << endl;
    cout << "Codigo estadia: " << codigoEstadia << endl;
    cout << "Quarto: " << numeroQuarto << endl;
    cout << "Diarias: " << quantidadeDiarias << endl;
    cout << "Valor total: R$ " << fixed << setprecision(2) << valorTotal << endl;
    
    if (index != -1) {
        estadias.erase(estadias.begin() + index);
    }
    
    salvarQuartos();
    salvarEstadias();
    
    cout << "\nBaixa realizada com sucesso!" << endl;
}

void pesquisarCliente() {
    cout << "\n=== PESQUISA DE CLIENTE ===" << endl;
    string busca;
    cout << "Digite nome ou codigo: ";
    limparBuffer();
    getline(cin, busca);
    
    bool encontrado = false;
    
    for (size_t i = 0; i < clientes.size(); i++) {
        string codigoStr = to_string(clientes[i].codigo);
        string nomeStr = paraString(clientes[i].nome);
        
        if (codigoStr == busca || nomeStr.find(busca) != string::npos) {
            cout << "\nCodigo: " << clientes[i].codigo << endl;
            cout << "Nome: " << clientes[i].nome << endl;
            cout << "Endereco: " << clientes[i].endereco << endl;
            cout << "Telefone: " << clientes[i].telefone << endl;
            cout << "-------------------" << endl;
            encontrado = true;
        }
    }
    
    if (!encontrado) {
        cout << "Cliente nao encontrado!" << endl;
    }
}

void pesquisarFuncionario() {
    cout << "\n=== PESQUISA DE FUNCIONARIO ===" << endl;
    string busca;
    cout << "Digite nome ou codigo: ";
    limparBuffer();
    getline(cin, busca);
    
    bool encontrado = false;
    
    for (size_t i = 0; i < funcionarios.size(); i++) {
        string codigoStr = to_string(funcionarios[i].codigo);
        string nomeStr = paraString(funcionarios[i].nome);
        
        if (codigoStr == busca || nomeStr.find(busca) != string::npos) {
            cout << "\nCodigo: " << funcionarios[i].codigo << endl;
            cout << "Nome: " << funcionarios[i].nome << endl;
            cout << "Telefone: " << funcionarios[i].telefone << endl;
            cout << "Cargo: " << funcionarios[i].cargo << endl;
            cout << "Salario: R$ " << fixed << setprecision(2) << funcionarios[i].salario << endl;
            cout << "-------------------" << endl;
            encontrado = true;
        }
    }
    
    if (!encontrado) {
        cout << "Funcionario nao encontrado!" << endl;
    }
}

void mostrarEstadiasCliente() {
    cout << "\n=== ESTADIAS DO CLIENTE ===" << endl;
    string busca;
    cout << "Digite nome ou codigo do cliente: ";
    limparBuffer();
    getline(cin, busca);
    
    vector<int> codigosClientes;
    for (size_t i = 0; i < clientes.size(); i++) {
        string codigoStr = to_string(clientes[i].codigo);
        string nomeStr = paraString(clientes[i].nome);
        
        if (codigoStr == busca || nomeStr.find(busca) != string::npos) {
            codigosClientes.push_back(clientes[i].codigo);
        }
    }
    
    if (codigosClientes.empty()) {
        cout << "Cliente nao encontrado!" << endl;
        return;
    }
    
    bool encontrado = false;
    for (size_t i = 0; i < estadias.size(); i++) {
        for (size_t j = 0; j < codigosClientes.size(); j++) {
            if (estadias[i].codigoCliente == codigosClientes[j]) {
                cout << "\nCodigo estadia: " << estadias[i].codigo << endl;
                cout << "Data entrada: " << estadias[i].dataEntrada << endl;
                cout << "Data saida: " << estadias[i].dataSaida << endl;
                cout << "Diarias: " << estadias[i].quantidadeDiarias << endl;
                cout << "Quarto: " << estadias[i].numeroQuarto << endl;
                cout << "-------------------" << endl;
                encontrado = true;
                break;
            }
        }
    }
    
    if (!encontrado) {
        cout << "Nenhuma estadia encontrada para este cliente!" << endl;
    }
}

void calcularPontosFidelidade() {
    cout << "\n=== PONTOS DE FIDELIDADE ===" << endl;
    string busca;
    cout << "Digite nome ou codigo do cliente: ";
    limparBuffer();
    getline(cin, busca);
    
    vector<int> codigosClientes;
    for (size_t i = 0; i < clientes.size(); i++) {
        string codigoStr = to_string(clientes[i].codigo);
        string nomeStr = paraString(clientes[i].nome);
        
        if (codigoStr == busca || nomeStr.find(busca) != string::npos) {
            codigosClientes.push_back(clientes[i].codigo);
        }
    }
    
    if (codigosClientes.empty()) {
        cout << "Cliente nao encontrado!" << endl;
        return;
    }
    
    int totalPontos = 0;
    for (size_t i = 0; i < estadias.size(); i++) {
        for (size_t j = 0; j < codigosClientes.size(); j++) {
            if (estadias[i].codigoCliente == codigosClientes[j]) {
                totalPontos += estadias[i].quantidadeDiarias * 10;
                break;
            }
        }
    }
    
    cout << "Total de pontos de fidelidade: " << totalPontos << endl;
}

void menu() {
    int opcao;
    
    do {
        cout << "\n=== SISTEMA HOTEL DESCANSO GARANTIDO ===" << endl;
        cout << "1. Cadastrar cliente" << endl;
        cout << "2. Cadastrar funcionario" << endl;
        cout << "3. Cadastrar quarto" << endl;
        cout << "4. Cadastrar estadia" << endl;
        cout << "5. Dar baixa em estadia" << endl;
        cout << "6. Pesquisar cliente" << endl;
        cout << "7. Pesquisar funcionario" << endl;
        cout << "8. Mostrar estadias do cliente" << endl;
        cout << "9. Calcular pontos de fidelidade" << endl;
        cout << "0. Sair" << endl;
        cout << "\nEscolha uma opcao: ";
        
        if (!(cin >> opcao)) {
            cout << "Opcao invalida!" << endl;
            limparBuffer();
            continue;
        }
        
        switch (opcao) {
            case 1: cadastrarCliente(); break;
            case 2: cadastrarFuncionario(); break;
            case 3: cadastrarQuarto(); break;
            case 4: cadastrarEstadia(); break;
            case 5: darBaixaEstadia(); break;
            case 6: pesquisarCliente(); break;
            case 7: pesquisarFuncionario(); break;
            case 8: mostrarEstadiasCliente(); break;
            case 9: calcularPontosFidelidade(); break;
            case 0: 
                cout << "Saindo... Dados salvos." << endl;
                break;
            default: 
                cout << "Opcao invalida!" << endl;
        }
        
    } while (opcao != 0);
}