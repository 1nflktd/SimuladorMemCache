/*
----------------------------------------------------
Arquivo: mem_cache.cpp
Autores: Henrique Scalon
		 Leonardo Invernizzi
----------------------------------------------------
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <Windows.h>
#include <iomanip>

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

	LPCSTR ini = "C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\config.ini";

	int tamanhoBloco = GetPrivateProfileIntA("config", "tamanhoBloco", 0, ini);
	
	int numeroBlocos = GetPrivateProfileIntA("config", "numeroBlocos", 0, ini);

	int numeroBlocosConjunto = GetPrivateProfileIntA("config", "numeroBlocosConjunto", 0, ini);

	int politicaEscrita = GetPrivateProfileIntA("config", "politicaEscrita", 0, ini);

	int tempoAcesso = GetPrivateProfileIntA("config", "tempoAcesso", 0, ini);

	int tempoAcessoMem = GetPrivateProfileIntA("config", "tempoAcessoMem", 0, ini);

	int politicaSubstituicao = GetPrivateProfileIntA("config", "politicaSubstituicao", 0, ini);

	int tamanhoCache = tamanhoBloco * numeroBlocos;
	int numeroConjuntos = tamanhoCache / (numeroBlocosConjunto * tamanhoBloco);

	std::vector<std::vector<SCache> > cache(numeroConjuntos, std::vector<SCache>(numeroBlocosConjunto));

	int endereco, verificacoes = 0, acertos = 0, erros = 0, enderecosEscrita = 0,
		leiturasMemoria = 0, escritasMemoria = 0;
	char tipoLeituraEscrita;

	//std::ifstream arq("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\teste.cache");
	std::ifstream arq("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\oficial.cache");
	//std::ofstream resTeste("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\resTeste.txt");
	//std::ifstream arq("teste.cache");

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
			//resTeste << std::hex << endereco << " " << tipoLeituraEscrita << " Cache" << std::endl;
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
			//resTeste << std::hex << endereco << " " << tipoLeituraEscrita << " MEM" << std::endl;
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

	std::ofstream res("C:\\Users\\W7\\Source\\Repos\\SimuladorMemCache\\SimuladorMemCache\\Debug\\res.txt");
	//std::ofstream res("res.txt");
	
	double taxaAcerto = double(acertos) / double(verificacoes);
	int tamCache = pow(2, log2(tamanhoBloco) + log2(numeroBlocos));

	res
		<< std::setprecision(4) << std::fixed
		<< "Tamanho da cache: " << tamCache << std::endl << std::endl
		<< "Parametros de entrada: " << std::endl << std::endl
		<< "Tamanho do bloco: " << tamanhoBloco << " bytes" << std::endl
		<< "Numero de blocos: " << numeroBlocos << std::endl
		<< "Blocos por conjunto: " << numeroBlocosConjunto << std::endl
		<< "Politica de escrita (0 - write-through, 1 - write-back): " << politicaEscrita << std::endl
		<< "Tempo de acesso: " << tempoAcesso << "ns" << std::endl
		<< "Tempo de acesso MEM: " << tempoAcessoMem << "ns" <<std::endl
		<< "Politica de substituicao (0 - LRU , 1 - aleatoria): " << politicaSubstituicao << std::endl << std::endl

		<< "Resultados: " << std::endl << std::endl

		<< "Total de endereços: " << verificacoes << ". " << enderecosEscrita << " de escrita. " << (verificacoes - enderecosEscrita) << " de leitura." << std::endl

		<< "Total de escritas da memoria principal: " << escritasMemoria << std::endl
		<< "Total de leituras da memoria principal: " << leiturasMemoria << std::endl
		<< "Total de acessos da memoria principal: " << leiturasMemoria + escritasMemoria << std::endl

		// Taxa de acerto (hit rate): especificar esta taxa por leitura, escrita e global (colocar ao lado a quantidade);

		<< "Taxa de acerto (hit rate): " << taxaAcerto * 100 << "%" << std::endl

		// Tempo médio de acesso da cache (em ns): utilizar a fórmula vista em aula;

		<< "Tempo médio de acesso: " << taxaAcerto * tempoAcesso + (1 - taxaAcerto) * (tempoAcesso + tempoAcessoMem) << std::endl

		//<< "Numero de acertos: " << acertos << std::endl
		//<< "Numero de erros: " << erros << std::endl
		;

	return 0;
}
