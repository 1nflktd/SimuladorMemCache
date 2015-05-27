#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define WT 0
#define WB 1
#define LRU 0
#define RAND 1


struct SCache {
	int rotulo = -1;
	int dirtybit = -1;
};

using namespace std;

int main()
{
	int tamanhoBloco;
	cout << "Tamanho do bloco (potencia de 2, em bytes): ";
	cin >> tamanhoBloco;

	int numeroBlocos;
	cout << "Número de blocos (potencia de 2): ";
	cin >> numeroBlocos;

	int numeroBlocosConjunto;
	cout << "Número de blocos por conjunto: ";
	cin >> numeroBlocosConjunto;

	int politicaEscrita;
	cout << "Politica de escrita (0 - write-through, 1 - write-back): ";
	cin >> politicaEscrita;

	int tempoLeitura;
	cout << "Tempo de leitura (em ns): ";
	cin >> tempoLeitura;

	int tempoEscrita;
	cout << "Tempo de escrita (em ns): ";
	cin >> tempoEscrita;

	int tempoAcesso;
	cout << "Tempo de acesso cache (em ns): ";
	cin >> tempoAcesso;

	int tempoAcessoMem;
	cout << "Tempo de acesso MEM (em ns): ";
	cin >> tempoAcessoMem;

	int politicaSubstituicao;
	cout << "Politica de substituicao (0 - LRU , 1 - aleatoria): ";
	cin >> politicaSubstituicao;

	int tamanhoCache = tamanhoBloco * numeroBlocos;
	int numeroConjuntos = tamanhoCache / (numeroBlocosConjunto * tamanhoBloco);

	vector<vector<SCache>> cache(numeroConjuntos, vector<SCache>(numeroBlocosConjunto));
	vector<vector<int>> ordemLRU(numeroConjuntos, vector<int>(numeroBlocosConjunto, -1));

	int endereco, verificacoes = 0, acertos = 0, erros = 0, enderecosEscrita = 0,
		leiturasMemoria = 0, escritasMemoria = 0;
	char tipoLeituraEscrita;

	ifstream arq("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\teste.cache");
	//ifstream arq("teste.cache");

	int conjunto;
	while (arq >> hex >> endereco >> tipoLeituraEscrita)
	{
		if (tipoLeituraEscrita == 'W')
			enderecosEscrita++;
		verificacoes++;
		conjunto = (endereco / tamanhoBloco) % numeroConjuntos;
		int rotulo = endereco / (tamanhoBloco * numeroConjuntos);
		size_t i = 0;
		while (i < numeroBlocosConjunto && cache[conjunto][i].rotulo != rotulo)
			i++;

		if (i < numeroBlocosConjunto && cache[conjunto][i].rotulo == rotulo)
		{
			acertos++;
			if (politicaSubstituicao == LRU)
			{
				struct SCache tmp = cache[conjunto][i];
				for (int j = i; j > 0; j--)
				{
					cache[conjunto][j] = cache[conjunto][j - 1];
				}
				i = 0;
				cache[conjunto][i] = tmp;
			}
			if (tipoLeituraEscrita == 'W')
			{
				if (politicaEscrita == WT) // write through
				{
					// escreve na memoria
					escritasMemoria++;
				}
				else // write back
				{
					cache[conjunto][i].dirtybit = 1;
				}
			}
		}
		else
		{
			erros++;
			int id;
			if (cache[conjunto][0].rotulo < 0)
			{
				id = numeroBlocosConjunto - 1;
				while (cache[conjunto][id].rotulo >= 0)
				{
					id--;
				}
			}
			else if (politicaSubstituicao == RAND)
			{
				id = rand() % numeroBlocosConjunto;
				if (cache[conjunto][id].dirtybit == 1)
				{
					escritasMemoria++;
				}
			}
			else
			{
				id = 0;
				if (cache[conjunto][numeroBlocosConjunto - 1].dirtybit == 1)
				{
					escritasMemoria++;
				}
				for (size_t i = numeroBlocosConjunto - 1; i > 0; i--)
				{
					cache[conjunto][i] = cache[conjunto][i - 1];
				}
			}
			cache[conjunto][id].rotulo = rotulo;

			// total de leituras da memoria
			leiturasMemoria++;

			if (politicaEscrita == WT && tipoLeituraEscrita == 'W')
			{
				escritasMemoria++;
			}
			else if (politicaEscrita == WB)
			{
				cache[conjunto][id].dirtybit = (tipoLeituraEscrita == 'W');
			}
		}
	}

	if (politicaEscrita == WB) 
	{
		for (size_t i = 0; i < numeroBlocosConjunto; i++)
		{
			if (cache[conjunto][i].dirtybit == 1)
			{
				escritasMemoria++;
			}
		}
	}

	ofstream res("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\res.txt");
	//ofstream res("res.txt");
	double taxaAcerto = double(acertos) / double(verificacoes);
	res
		<< "Parametros de entrada: " << endl
		<< "Tamanho do bloco " << tamanhoBloco << endl
		<< "Numero de blocos " << numeroBlocos << endl
		<< "Blocos por conjunto " << numeroBlocosConjunto << endl
		<< "Politica de escrita " << politicaEscrita << endl
		<< "Tempo de leitura (em ns) " << tempoLeitura << endl
		<< "Tempo de escrita (em ns) " << tempoEscrita << endl
		<< "Tempo de acesso (em ns) " << tempoAcesso << endl
		<< "Politica de substituicao (0 - LRU , 1 - aleatoria) " << politicaSubstituicao << endl

		<< "Total de endereços: " << verificacoes << ". " << enderecosEscrita << " de escrita. " << (verificacoes - enderecosEscrita) << " de leitura." << endl

		<< "Total de escritas da memoria principal: " << escritasMemoria << endl
		<< "Total de leituras da memoria principal: " << leiturasMemoria << endl

		// Taxa de acerto (hit rate): especificar esta taxa por leitura, escrita e global (colocar ao lado a quantidade);

		<< "Taxa de acerto (hit rate): " << taxaAcerto * 100 << "%" << endl

		// Tempo médio de acesso da cache (em ns): utilizar a fórmula vista em aula;

		<< "Tempo médio de acesso: " << taxaAcerto * tempoAcesso + (1 - taxaAcerto) * (tempoAcesso + tempoAcessoMem) << endl

		//<< "Numero de acertos: " << acertos << endl
		//<< "Numero de erros: " << erros << endl
		;

	return 0;
}
