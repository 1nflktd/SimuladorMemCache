#include <iostream>
#include <vector>
#include <fstream>
#include <string>

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
	/*
	int politicaEscrita;
	std::cout << "Politica de escrita (1 - write-back, 0 - write-through): ";
	std::cin >> politicaEscrita;

	int tempoLeitura;
	std::cout << "Tempo de leitura (em ns): ";
	std::cin >> tempoLeitura;

	int tempoEscrita;
	std::cout << "Tempo de escrita (em ns): ";
	std::cin >> tempoEscrita;

	int tempoAcesso;
	std::cout << "Tempo de acesso (em ns): ";
	std::cin >> tempoAcesso;

	int politicaSubstituicao;
	std::cout << "Politica de substituicao (0 - LRU , 1 - aleatoria): ";
	std::cin >> politicaSubstituicao;
	*/

	int tamanhoCache = tamanhoBloco * numeroBlocos;
	int numeroConjuntos = tamanhoCache / (numeroBlocosConjunto * tamanhoBloco);

	std::vector<std::vector<int>> cache(numeroConjuntos, std::vector<int>(numeroBlocosConjunto));

	int endereco, verificacoes = 0, acertos = 0, erros = 0;
	char tipoLeituraEscrita;

	std::ifstream arq("teste.cache");
	while (arq >> std::hex >> endereco >> tipoLeituraEscrita)
	{
		verificacoes++;
		int conjunto = (endereco / tamanhoBloco) % numeroConjuntos;
		int rotulo = endereco / (tamanhoBloco * numeroConjuntos);
		size_t i = 0;
		while (i < numeroBlocosConjunto && cache[conjunto][i] != rotulo)
			i++;
		
		if (i < numeroBlocosConjunto && cache[conjunto][i] == rotulo)
		{
			acertos++;
		}
		else
		{
			erros++;
			int aleatorio = rand() % numeroBlocosConjunto;
			cache[conjunto][aleatorio] = rotulo;
		}
	}

	std::cout 
		<< "Numero de verificacoes: " << verificacoes << std::endl
		<< "Numero de acertos: " << acertos << std::endl
		<< "Numero de erros: " << erros << std::endl
		;

	return 0;
}
