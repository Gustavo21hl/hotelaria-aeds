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

// ==========================================
// 1. ESTRUTURAS DE DADOS (MODEL)
// Mantivemos char arrays para facilitar a persistência binária
// ==========================================
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
    char status[20]; // "desocupado", "ocupado"
};

struct Estadia {
    int codigo;
    char dataEntrada[11]; // YYYY-MM-DD
    char dataSaida[11];
    int quantidadeDiarias;
    int codigoCliente;
    int numeroQuarto;
    double valorTotal; // Adicionado para histórico
};

// ==========================================
// 2. UTILITÁRIOS (Helpers)
// ==========================================
namespace Utils {
    // Limpa o buffer de entrada de forma segura
    void limparBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Lê strings com segurança (incluindo espaços)
    void lerString(char* buffer, int tamanho) {
        cin.getline(buffer, tamanho);
        if (cin.fail()) {
            limparBuffer();
        }
    }

    // Template para ler números com validação de tipo
    template <typename T>
    T lerNumero(const string& mensagem) {
        T valor;
        while (true) {
            cout << mensagem;
            if (cin >> valor) {
                limparBuffer(); // Consome o \n que sobra
                return valor;
            } else {
                cout << "Entrada invalida! Por favor, digite um numero." << endl;
                limparBuffer();
            }
        }
    }

    // Funções de Data
    int calcularDiferencaDias(const string& entrada, const string& saida) {
        int ano1, mes1, dia1, ano2, mes2, dia2;
        if (sscanf(entrada.c_str(), "%d-%d-%d", &ano1, &mes1, &dia1) != 3) return -1;
        if (sscanf(saida.c_str(), "%d-%d-%d", &ano2, &mes2, &dia2) != 3) return -1;

        struct tm tm1 = {0}, tm2 = {0};
        tm1.tm_year = ano1 - 1900; tm1.tm_mon = mes1 - 1; tm1.tm_mday = dia1;
        tm2.tm_year = ano2 - 1900; tm2.tm_mon = mes2 - 1; tm2.tm_mday = dia2;

        time_t t1 = mktime(&tm1);
        time_t t2 = mktime(&tm2);

        if (t1 == -1 || t2 == -1) return -1;

        double segundos = difftime(t2, t1);
        return (int)(segundos / (60 * 60 * 24));
    }
    
    string dataAtual() {
        time_t t = time(0);
        struct tm* now = localtime(&t);
        char buf[11];
        sprintf(buf, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
        return string(buf);
    }
}

// ==========================================
// 3. CLASSE DO SISTEMA (CONTROLLER)
// ==========================================
class SistemaHotel {
private:
    vector<Cliente> clientes;
    vector<Funcionario> funcionarios;
    vector<Quarto> quartos;
    vector<Estadia> estadias;

    const string ARQ_CLIENTES = "clientes.dat";
    const string ARQ_FUNCIONARIOS = "funcionarios.dat";
    const string ARQ_QUARTOS = "quartos.dat";
    const string ARQ_ESTADIAS = "estadias.dat";

    // Métodos Genéricos de Arquivo (Template para evitar repetição de código)
    template <typename T>
    void salvarVetor(const vector<T>& vetor, const string& nomeArquivo) {
        ofstream arquivo(nomeArquivo.c_str(), ios::binary);
        if (arquivo) {
            for (const auto& item : vetor) {
                arquivo.write((const char*)&item, sizeof(T));
            }
        }
    }

    template <typename T>
    void carregarVetor(vector<T>& vetor, const string& nomeArquivo) {
        ifstream arquivo(nomeArquivo.c_str(), ios::binary);
        if (arquivo) {
            T item;
            while (arquivo.read((char*)&item, sizeof(T))) {
                vetor.push_back(item);
            }
        }
    }

    int gerarNovoId(const vector<Cliente>& v) { return v.empty() ? 1 : v.back().codigo + 1; }
    int gerarNovoId(const vector<Funcionario>& v) { return v.empty() ? 1 : v.back().codigo + 1; }
    int gerarNovoId(const vector<Estadia>& v) { return v.empty() ? 1 : v.back().codigo + 1; }

public:
    SistemaHotel() {
        carregarDados();
    }

    ~SistemaHotel() {
        salvarDados();
    }

    void carregarDados() {
        carregarVetor(clientes, ARQ_CLIENTES);
        carregarVetor(funcionarios, ARQ_FUNCIONARIOS);
        carregarVetor(quartos, ARQ_QUARTOS);
        carregarVetor(estadias, ARQ_ESTADIAS);
    }

    void salvarDados() {
        salvarVetor(clientes, ARQ_CLIENTES);
        salvarVetor(funcionarios, ARQ_FUNCIONARIOS);
        salvarVetor(quartos, ARQ_QUARTOS);
        salvarVetor(estadias, ARQ_ESTADIAS);
    }

    // --- Gestão de Clientes ---
    void adicionarCliente(Cliente c) {
        c.codigo = gerarNovoId(clientes);
        clientes.push_back(c);
        cout << "Cliente cadastrado com ID: " << c.codigo << endl;
    }

    const vector<Cliente>& getClientes() const { return clientes; }

    Cliente* buscarCliente(int codigo) {
        for (auto& c : clientes) {
            if (c.codigo == codigo) return &c;
        }
        return nullptr;
    }

    // --- Gestão de Funcionários ---
    void adicionarFuncionario(Funcionario f) {
        f.codigo = gerarNovoId(funcionarios);
        funcionarios.push_back(f);
        cout << "Funcionario cadastrado com ID: " << f.codigo << endl;
    }

    const vector<Funcionario>& getFuncionarios() const { return funcionarios; }

    // --- Gestão de Quartos ---
    bool adicionarQuarto(Quarto q) {
        for (const auto& quarto : quartos) {
            if (quarto.numero == q.numero) return false; // Já existe
        }
        strcpy(q.status, "desocupado");
        quartos.push_back(q);
        return true;
    }

    // --- Lógica de Estadias ---
    bool verificarDisponibilidade(int numeroQuarto, string entrada, string saida) {
        // Verifica se o quarto existe e se está livre fisicamente agora (opcional)
        // Mas o mais importante é checar choque de datas nas estadias futuras
        
        for (const auto& est : estadias) {
            if (est.numeroQuarto == numeroQuarto) {
                // Lógica de intersecção de datas
                // Se (NovaEntrada < EstadiaSaida) E (NovaSaida > EstadiaEntrada) -> Colide
                string estEntrada = est.dataEntrada;
                string estSaida = est.dataSaida;
                
                if (entrada < estSaida && saida > estEntrada) {
                    return false; // Ocupado nessas datas
                }
            }
        }
        return true;
    }

    Quarto* buscarQuarto(int numero) {
        for (auto& q : quartos) {
            if (q.numero == numero) return &q;
        }
        return nullptr;
    }

    bool realizarCheckIn(int codCliente, int qtdHospedes, string dtEntrada, string dtSaida) {
        int dias = Utils::calcularDiferencaDias(dtEntrada, dtSaida);
        if (dias <= 0) {
            cout << "Erro: Data de saida deve ser posterior a entrada." << endl;
            return false;
        }

        if (buscarCliente(codCliente) == nullptr) {
            cout << "Erro: Cliente nao encontrado." << endl;
            return false;
        }

        // Tentar encontrar quarto
        int quartoEscolhido = -1;
        double valorDiaria = 0;

        for (auto& q : quartos) {
            if (q.capacidade >= qtdHospedes && 
                verificarDisponibilidade(q.numero, dtEntrada, dtSaida)) {
                
                quartoEscolhido = q.numero;
                valorDiaria = q.valorDiaria;
                // Atualiza status se a data for HOJE (opcional, simplificado)
                if (dtEntrada <= Utils::dataAtual() && dtSaida > Utils::dataAtual()) {
                    strcpy(q.status, "ocupado");
                }
                break;
            }
        }

        if (quartoEscolhido == -1) {
            cout << "Nao ha quartos disponiveis para essa capacidade/data." << endl;
            return false;
        }

        Estadia nova;
        nova.codigo = gerarNovoId(estadias);
        nova.codigoCliente = codCliente;
        nova.numeroQuarto = quartoEscolhido;
        nova.quantidadeDiarias = dias;
        nova.valorTotal = dias * valorDiaria;
        strcpy(nova.dataEntrada, dtEntrada.c_str());
        strcpy(nova.dataSaida, dtSaida.c_str());

        estadias.push_back(nova);
        cout << "Estadia agendada! Quarto: " << quartoEscolhido << " | Total Estimado: R$ " << nova.valorTotal << endl;
        return true;
    }

    bool realizarCheckOut(int codigoEstadia) {
        for (size_t i = 0; i < estadias.size(); i++) {
            if (estadias[i].codigo == codigoEstadia) {
                // Libera o status do quarto
                Quarto* q = buscarQuarto(estadias[i].numeroQuarto);
                if (q) strcpy(q->status, "desocupado");

                cout << "Check-out realizado. Valor Final: R$ " << fixed << setprecision(2) 
                     << estadias[i].valorTotal << endl;
                
                // Remove do vetor de estadias ATIVAS (se quiser manter histórico, mova para outro vetor)
                estadias.erase(estadias.begin() + i);
                return true;
            }
        }
        return false;
    }

    void listarEstadiasCliente(int codCliente) {
        bool achou = false;
        cout << "\n--- Estadias do Cliente " << codCliente << " ---" << endl;
        for (const auto& e : estadias) {
            if (e.codigoCliente == codCliente) {
                cout << "Cod Estadia: " << e.codigo << " | Quarto: " << e.numeroQuarto 
                     << " | Entrada: " << e.dataEntrada << " | Saida: " << e.dataSaida << endl;
                achou = true;
            }
        }
        if (!achou) cout << "Nenhuma estadia encontrada." << endl;
    }
};

// ==========================================
// 4. INTERFACE DO USUÁRIO (VIEW)
// ==========================================
void menuCadastrarCliente(SistemaHotel& sys) {
    Cliente c;
    cout << "\n=== NOVO CLIENTE ===" << endl;
    cout << "Nome: "; Utils::lerString(c.nome, 100);
    cout << "Endereco: "; Utils::lerString(c.endereco, 200);
    cout << "Telefone: "; Utils::lerString(c.telefone, 20);
    sys.adicionarCliente(c);
}

void menuCadastrarFuncionario(SistemaHotel& sys) {
    Funcionario f;
    cout << "\n=== NOVO FUNCIONARIO ===" << endl;
    cout << "Nome: "; Utils::lerString(f.nome, 100);
    cout << "Telefone: "; Utils::lerString(f.telefone, 20);
    cout << "Cargo: "; Utils::lerString(f.cargo, 50);
    f.salario = Utils::lerNumero<double>("Salario: ");
    sys.adicionarFuncionario(f);
}

void menuCadastrarQuarto(SistemaHotel& sys) {
    Quarto q;
    cout << "\n=== NOVO QUARTO ===" << endl;
    q.numero = Utils::lerNumero<int>("Numero do Quarto: ");
    q.capacidade = Utils::lerNumero<int>("Capacidade de Pessoas: ");
    q.valorDiaria = Utils::lerNumero<double>("Valor da Diaria: ");
    
    if (sys.adicionarQuarto(q)) {
        cout << "Quarto cadastrado com sucesso!" << endl;
    } else {
        cout << "Erro: Ja existe um quarto com esse numero." << endl;
    }
}

void menuNovaEstadia(SistemaHotel& sys) {
    cout << "\n=== NOVA RESERVA / CHECK-IN ===" << endl;
    int codCli = Utils::lerNumero<int>("Codigo do Cliente: ");
    int qtd = Utils::lerNumero<int>("Quantidade de Hospedes: ");
    
    char entrada[11], saida[11];
    cout << "Data Entrada (YYYY-MM-DD): "; Utils::lerString(entrada, 11);
    cout << "Data Saida (YYYY-MM-DD): "; Utils::lerString(saida, 11);

    sys.realizarCheckIn(codCli, qtd, string(entrada), string(saida));
}

void menuBaixaEstadia(SistemaHotel& sys) {
    cout << "\n=== CHECK-OUT / BAIXA ===" << endl;
    int cod = Utils::lerNumero<int>("Codigo da Estadia: ");
    if (!sys.realizarCheckOut(cod)) {
        cout << "Estadia nao encontrada!" << endl;
    }
}

void menuPesquisar(SistemaHotel& sys) {
    cout << "\n=== PESQUISA ===" << endl;
    cout << "1. Listar Clientes\n2. Listar Funcionarios\nOpcao: ";
    int op = Utils::lerNumero<int>("");
    
    if (op == 1) {
        for (const auto& c : sys.getClientes()) {
            cout << "ID: " << c.codigo << " | Nome: " << c.nome << endl;
        }
    } else if (op == 2) {
        for (const auto& f : sys.getFuncionarios()) {
            cout << "ID: " << f.codigo << " | Nome: " << f.nome << " | Cargo: " << f.cargo << endl;
        }
    }
}

// ==========================================
// 5. PROGRAMA PRINCIPAL
// ==========================================
int main() {
    SistemaHotel hotel; // Instância única que gerencia tudo
    int opcao;

    do {
        cout << "\n===================================" << endl;
        cout << "   HOTEL DESCANSO GARANTIDO v2.0   " << endl;
        cout << "===================================" << endl;
        cout << "1. Cadastrar Cliente" << endl;
        cout << "2. Cadastrar Funcionario" << endl;
        cout << "3. Cadastrar Quarto" << endl;
        cout << "4. Realizar Reserva (Check-In)" << endl;
        cout << "5. Realizar Baixa (Check-Out)" << endl;
        cout << "6. Listar/Pesquisar" << endl;
        cout << "7. Ver Estadias de um Cliente" << endl;
        cout << "0. Sair" << endl;
        
        opcao = Utils::lerNumero<int>("Escolha uma opcao: ");

        switch (opcao) {
            case 1: menuCadastrarCliente(hotel); break;
            case 2: menuCadastrarFuncionario(hotel); break;
            case 3: menuCadastrarQuarto(hotel); break;
            case 4: menuNovaEstadia(hotel); break;
            case 5: menuBaixaEstadia(hotel); break;
            case 6: menuPesquisar(hotel); break;
            case 7: {
                int id = Utils::lerNumero<int>("ID do Cliente: ");
                hotel.listarEstadiasCliente(id);
                break;
            }
            case 0: cout << "Salvando e saindo..." << endl; break;
            default: cout << "Opcao invalida!" << endl;
        }
    } while (opcao != 0);

    return 0;
}