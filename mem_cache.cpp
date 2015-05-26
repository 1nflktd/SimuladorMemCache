#include <iostream>
#include <vector>
#include <fstream>
#include <string>

struct SCache {
	int rotulo;
	int dirtybit;
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
	cout << "Politica de escrita (1 - write-back, 0 - write-through): ";
	cin >> politicaEscrita;

	int tempoLeitura;
	cout << "Tempo de leitura (em ns): ";
	cin >> tempoLeitura;

	int tempoEscrita;
	cout << "Tempo de escrita (em ns): ";
	cin >> tempoEscrita;

	int tempoAcesso;
	cout << "Tempo de acesso (em ns): ";
	cin >> tempoAcesso;

	int politicaSubstituicao;
	cout << "Politica de substituicao (0 - LRU , 1 - aleatoria): ";
	cin >> politicaSubstituicao;

	int tamanhoCache = tamanhoBloco * numeroBlocos;
	int numeroConjuntos = tamanhoCache / (numeroBlocosConjunto * tamanhoBloco);

	vector<vector<SCache>> cache(numeroConjuntos, vector<SCache>(numeroBlocosConjunto));
	vector<vector<int>> ordemLRU(numeroConjuntos, vector<int>(numeroBlocosConjunto));

	int endereco, verificacoes = 0, acertos = 0, erros = 0, enderecosEscrita = 0,
		leiturasMemoria = 0, escritasMemoria = 0;
	char tipoLeituraEscrita;

	//ifstream arq("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\teste.cache");
	ifstream arq("teste.cache");
	while (arq >> hex >> endereco >> tipoLeituraEscrita)
	{
		if (tipoLeituraEscrita == 'W')
			enderecosEscrita++;
		verificacoes++;
		int conjunto = (endereco / tamanhoBloco) % numeroConjuntos;
		int rotulo = endereco / (tamanhoBloco * numeroConjuntos);
		size_t i = 0;
		while (i < numeroBlocosConjunto && cache[conjunto][i].rotulo != rotulo)
			i++;

		if (i < numeroBlocosConjunto && cache[conjunto][i].rotulo == rotulo)
		{
			acertos++;
			if (tipoLeituraEscrita == 'W')
			{
				if (politicaEscrita == 0) // write through
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
			if (cache[conjunto][i].dirtybit == 1) 
			{
				escritasMemoria++;
			}
			int id;
			if (!cache[conjunto][numeroBlocosConjunto].rotulo) {
//				id = cache[conjunto].size - 1;
			} 
			else if (politicaSubstituicao == 1)
			{
				id = rand() % numeroBlocosConjunto;
			}
			else 
			{
				//LRU
			}
			cache[conjunto][id].rotulo = rotulo;
			cache[conjunto][id].dirtybit = 0;
			// total de leituras da memoria
			leiturasMemoria++;
		}
	}

	ofstream res("res.txt");
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

		<< "Taxa de acerto (hit rate): " << double(acertos)/double(verificacoes) << endl

		// Tempo médio de acesso da cache (em ns): utilizar a fórmula vista em aula;

		//<< "Numero de acertos: " << acertos << endl
		//<< "Numero de erros: " << erros << endl
	;

	return 0;
}
