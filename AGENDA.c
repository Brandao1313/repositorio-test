a
} Data;

typedef struct {
    Data data;
    int hora;
    int minuto;
    int duracao;
    char descricao[100];
    int ativo;
} Compromisso;

Compromisso agenda[MAX_COMPROMISSOS];
int totalCompromissos = 0+1;
Data dataAtual;

// função para limpar o buffer de entrada
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

long long dataParaMinutos(Data data, int hora, int minuto) {
    long long totalMinutos = 0;
    totalMinutos += (long long)data.ano * 525960;
    totalMinutos += (long long)data.mes * 43833;
    totalMinutos += (long long)data.dia * 1440;
    totalMinutos += (long long)hora * 60;
    totalMinutos += minuto;
    return totalMinutos;
}

int ValidarDados(Data data) {
    if (data.ano < dataAtual.ano) return 0; // Ano passado
    if (data.mes < 1 || data.mes > 12) return 0; // Mês inválido
    if (data.dia < 1) return 0; // Dia inválido

    int diasNoMes = 31;
    if (data.mes == 4 || data.mes == 6 || data.mes == 9 || data.mes == 11) {
        diasNoMes = 30;
    } else if (data.mes == 2) {
        // Verifica se é ano bissexto
        if ((data.ano % 4 == 0 && data.ano % 100 != 0) || (data.ano % 400 == 0)) {
            diasNoMes = 29;
        } else {
            diasNoMes = 28;
        }
    }

    if (data.dia > diasNoMes) return 0; // Dia inválido para o mês

    // Checagem final contra a data atual
    if (data.ano == dataAtual.ano && data.mes < dataAtual.mes) return 0;
    if (data.ano == dataAtual.ano && data.mes == dataAtual.mes && data.dia < dataAtual.dia) return 0;

    return 1;
}

// --- Funções de Leitura e Validação por Etapa (NOVO) ---

Data lerDataValida() {
    Data data;
    while (1) {
        printf("Data (DD/MM/AAAA): ");
        if (scanf("%d/%d/%d", &data.dia, &data.mes, &data.ano) == 3) {
            if (ValidarDados(data)) {
                limparBuffer();
                return data;
            } else {
                printf("Erro: Data invalida ou no passado. Digite novamente.\n");
            }
        } else {
            printf("Erro: Formato de entrada invalido. Use DD/MM/AAAA.\n");
        }
        limparBuffer(); // Limpa a entrada inválida
    }
}

void lerHoraValida(int *hora, int *minuto) {
    while (1) {
        printf("Hora (HH:MM): ");
        if (scanf("%d:%d", hora, minuto) == 2) {
            if (*hora >= 0 && *hora <= 23 && *minuto >= 0 && *minuto <= 59) {
                limparBuffer();
                return;
            } else {
                printf("Erro: Hora ou minuto invalido. Digite novamente.\n");
            }
        } else {
            printf("Erro: Formato de entrada invalido. Use HH:MM.\n");
        }
        limparBuffer();
    }
}

int lerDuracaoValida() {
    int duracao;
    while (1) {
        printf("Duracao (em minutos): ");
        if (scanf("%d", &duracao) == 1) {
            if (duracao > 0) {
                limparBuffer();
                return duracao;
            } else {
                printf("Erro: A duracao deve ser um numero positivo. Tente novamente.\n");
            }
        } else {
            printf("Erro: Entrada invalida. Digite um numero.\n");
        }
        limparBuffer();
    }
}



void carregarCompromissos() {
    FILE *arquivo = fopen(NOME_ARQUIVO, "r");
    if (!arquivo) return; // Arquivo não existe, nada a carregar

    while (fscanf(arquivo, "%d/%d/%d %d:%d %d %[^\n]\n",
                  &agenda[totalCompromissos].data.dia,
                  &agenda[totalCompromissos].data.mes,
                  &agenda[totalCompromissos].data.ano,
                  &agenda[totalCompromissos].hora,
                  &agenda[totalCompromissos].minuto,
                  &agenda[totalCompromissos].duracao,
                  agenda[totalCompromissos].descricao) == 7) {
        agenda[totalCompromissos].ativo = 1;
        totalCompromissos++;
        if (totalCompromissos >= MAX_COMPROMISSOS) break; // Limite atingido
    }
    fclose(arquivo);
}
int verificaConflito(Data data, int hora, int minuto, int duracao, int idIgnorado) {
    long long inicioNovo = dataParaMinutos(data, hora, minuto);
    long long fimNovo = inicioNovo + duracao;
    for (int i = 0; i < totalCompromissos; i++) {
        if (i == idIgnorado || !agenda[i].ativo) continue;
        long long inicioExistente = dataParaMinutos(agenda[i].data, agenda[i].hora, agenda[i].minuto);
        long long fimExistente = inicioExistente + agenda[i].duracao;
        if (inicioNovo < fimExistente && fimNovo > inicioExistente) {
            printf("\n--- CONFLITO DETECTADO ---\n");
            printf("O novo compromisso entra em conflito com o ID %d: %s\n", i, agenda[i].descricao);
            return 1;
        }
    }
    return 0;
}

void adicionarCompromisso() {
    if (totalCompromissos >= MAX_COMPROMISSOS) {
        printf("Erro: A agenda esta cheia!\n");
        return;
    }
    Compromisso novo;
    printf("\n--- Adicionar Novo Compromisso ---\n");

    
    novo.data = lerDataValida();
    lerHoraValida(&novo.hora, &novo.minuto);
    novo.duracao = lerDuracaoValida();

    printf("Descricao: ");
    fgets(novo.descricao, 100, stdin);
    novo.descricao[strcspn(novo.descricao, "\n")] = 0;

    // Validação final de conflito
    if (verificaConflito(novo.data, novo.hora, novo.minuto, novo.duracao, -1)) {
        return;
    }

    novo.ativo = 1;
    agenda[totalCompromissos] = novo;
    totalCompromissos++;
    printf("\nCompromisso adicionado com sucesso!\n");
}

void listarCompromissos() {
    printf("\n--- Lista de Todos os Compromissos ---\n");
    int encontrou = 0;
    for (int i = 0; i < totalCompromissos; i++) {
        if (agenda[i].ativo) {
            printf("ID: %d | Data: %02d/%02d/%04d | Hora: %02d:%02d | Dur: %d min | Desc: %s\n",
                   i, agenda[i].data.dia, agenda[i].data.mes, agenda[i].data.ano,
                   agenda[i].hora, agenda[i].minuto, agenda[i].duracao, agenda[i].descricao);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Nenhum compromisso cadastrado.\n");
}

void editarCompromisso() {
    int id;
    printf("\nDigite o ID do compromisso que deseja editar: ");
    scanf("%d", &id);
    limparBuffer();
    if (id < 0 || id >= totalCompromissos || !agenda[id].ativo) {
        printf("Erro: ID invalido ou compromisso nao encontrado.\n");
        return;
    }
    
    Compromisso editado;
    printf("\n--- Editando Compromisso ID %d ---\n", id);
    
    editado.data = lerDataValida();
    lerHoraValida(&editado.hora, &editado.minuto);
    editado.duracao = lerDuracaoValida();

    printf("Nova descricao (atual: %s): ", agenda[id].descricao);
    fgets(editado.descricao, 100, stdin);
    editado.descricao[strcspn(editado.descricao, "\n")] = 0;

    if (verificaConflito(editado.data, editado.hora, editado.minuto, editado.duracao, id)) {
        return;
    }
    
    editado.ativo = 1;
    agenda[id] = editado;
    printf("\nCompromisso editado com sucesso!\n");
}

void removerCompromisso() {
    int id;
    printf("\nDigite o ID do compromisso que deseja remover: ");
    scanf(" %d", &id);
    limparBuffer();
    if (id < 0 || id >= totalCompromissos || !agenda[id].ativo) {
        printf("Erro: ID invalido ou compromisso ja removido.\n");
        return;
    }
    agenda[id].ativo = 0;
    printf("Compromisso removido com sucesso!\n");
}

void filtrarPorData() {
    Data dataFiltro;
    printf("\n--- Filtrar por Data ---\n");
    dataFiltro = lerDataValida();

    printf("\n--- Compromissos em %02d/%02d/%04d ---\n", dataFiltro.dia, dataFiltro.mes, dataFiltro.ano);
    int encontrou = 0;
    for (int i = 0; i < totalCompromissos; i++) {
        if (agenda[i].ativo && agenda[i].data.dia == dataFiltro.dia &&
            agenda[i].data.mes == dataFiltro.mes && agenda[i].data.ano == dataFiltro.ano) {
            printf("ID: %d | Hora: %02d:%02d | Dur: %d min | Desc: %s\n",
                   i, agenda[i].hora, agenda[i].minuto, agenda[i].duracao, agenda[i].descricao);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Nenhum compromisso encontrado para esta data.\n");
}

void mostrarMenu() {
    printf("\n--- Agenda de Compromissos (Data de Referencia: %02d/%02d/%04d) ---\n", dataAtual.dia, dataAtual.mes, dataAtual.ano);
    printf("1. Adicionar Compromisso\n");
    printf("2. Listar Todos os Compromissos\n");
    printf("3. Editar Compromisso\n");
    printf("4. Remover Compromisso\n");
    printf("5. Filtrar Compromissos por Data\n");
    printf("0. Sair\n");
    printf("-----------------------------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

void definirDataAtual() {
    printf("--- Configuracao Inicial ---\n");
    dataAtual = lerDataValida();
    printf("Otimo! A data de referencia foi definida como %02d/%02d/%04d.\n", dataAtual.dia, dataAtual.mes, dataAtual.ano);
}

int main() {
    definirDataAtual();
    carregarCompromissos();
    int opcao;
    do {
        mostrarMenu();
        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Define uma opção inválida para o default
        }
        limparBuffer();

        switch (opcao) {
            case 1: adicionarCompromisso(); break;
            case 2: listarCompromissos(); break;
            case 3: editarCompromisso(); break;
            case 4: removerCompromisso(); break;
            case 5: filtrarPorData(); break;
            case 0: printf("Saindo e salvando alteracoes...\n"); break;
            default: printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);
   
    return 0;
}
