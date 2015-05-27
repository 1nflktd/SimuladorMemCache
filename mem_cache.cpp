#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define WT 0
#define WB 1
#define LRU 0
#define RAND 1

struct SCache 
{
	int rotulo = -1;
	int dirtybit = -1;
};

int main()
{
	int tamanhoBloco;
	std::cout << "Tamanho do bloco (potencia de 2, em bytes): ";
	std::cin >> tamanhoBloco;

	int numeroBlocos;
	std::cout << "Número de blocos (potencia de 2): ";
	std::cin >> numeroBlocos;

	int numeroBlocosConjunto;
	std::cout << "Número de blocos por conjunto: ";
	std::cin >> numeroBlocosConjunto;

	int politicaEscrita;
	std::cout << "Politica de escrita (0 - write-through, 1 - write-back): ";
	std::cin >> politicaEscrita;

	int tempoLeitura;
	std::cout << "Tempo de leitura (em ns): ";
	std::cin >> tempoLeitura;

	int tempoEscrita;
	std::cout << "Tempo de escrita (em ns): ";
	std::cin >> tempoEscrita;

	int tempoAcesso;
	std::cout << "Tempo de acesso cache (em ns): ";
	std::cin >> tempoAcesso;

	int tempoAcessoMem;
	std::cout << "Tempo de acesso MEM (em ns): ";
	std::cin >> tempoAcessoMem;

	int politicaSubstituicao;
	std::cout << "Politica de substituicao (0 - LRU , 1 - aleatoria): ";
	std::cin >> politicaSubstituicao;

	int tamanhoCache = tamanhoBloco * numeroBlocos;
	int numeroConjuntos = tamanhoCache / (numeroBlocosConjunto * tamanhoBloco);

	std::vector<std::vector<SCache> > cache(numeroConjuntos, std::vector<SCache>(numeroBlocosConjunto));
	std::vector<std::vector<int> > ordemLRU(numeroConjuntos, std::vector<int>(numeroBlocosConjunto, -1));

	int endereco, verificacoes = 0, acertos = 0, erros = 0, enderecosEscrita = 0,
		leiturasMemoria = 0, escritasMemoria = 0;
	char tipoLeituraEscrita;

	//std::ifstream arq("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\teste.cache");
	std::ifstream arq("teste.cache");

	int conjunto;
	while (arq >> std::hex >> endereco >> tipoLeituraEscrita)
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
	
	if(verificacoes == 0) 
	{
		std::cout << "Erro ao abrir arquivo da cache." << std::endl;
		return -1;
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

	//std::ofstream res("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\res.txt");
	std::ofstream res("res.txt");
	
	double taxaAcerto = double(acertos) / double(verificacoes);
	res
		<< "Parametros de entrada: " << std::endl
		<< "Tamanho do bloco " << tamanhoBloco << std::endl
		<< "Numero de blocos " << numeroBlocos << std::endl
		<< "Blocos por conjunto " << numeroBlocosConjunto << std::endl
		<< "Politica de escrita " << politicaEscrita << std::endl
		<< "Tempo de leitura (em ns) " << tempoLeitura << std::endl
		<< "Tempo de escrita (em ns) " << tempoEscrita << std::endl
		<< "Tempo de acesso (em ns) " << tempoAcesso << std::endl
		<< "Politica de substituicao (0 - LRU , 1 - aleatoria) " << politicaSubstituicao << std::endl

		<< "Total de endereços: " << verificacoes << ". " << enderecosEscrita << " de escrita. " << (verificacoes - enderecosEscrita) << " de leitura." << std::endl

		<< "Total de escritas da memoria principal: " << escritasMemoria << std::endl
		<< "Total de leituras da memoria principal: " << leiturasMemoria << std::endl

		// Taxa de acerto (hit rate): especificar esta taxa por leitura, escrita e global (colocar ao lado a quantidade);

		<< "Taxa de acerto (hit rate): " << taxaAcerto * 100 << "%" << std::endl

		// Tempo médio de acesso da cache (em ns): utilizar a fórmula vista em aula;

		<< "Tempo médio de acesso: " << taxaAcerto * tempoAcesso + (1 - taxaAcerto) * (tempoAcesso + tempoAcessoMem) << std::endl

		//<< "Numero de acertos: " << acertos << std::endl
		//<< "Numero de erros: " << erros << std::endl
		;

	return 0;
}
