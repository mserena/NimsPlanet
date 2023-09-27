//
//  TTContentCalculator
//  animalplanet
//
//  Created by Albert on 09/11/14.
//
//

#include "TTContentCalculator.h"

#define TYPE_RANDOM false

//singleton
static TTContentCalculator *_shared = nullptr;

TTContentCalculator* TTContentCalculator::getInstance()
{
    if (!_shared)
    {
        _shared = new TTContentCalculator();
        _shared->init();
    }
    
    return _shared;
}

void TTContentCalculator::destroyInstance()
{
    CC_SAFE_DELETE(_shared);
}

TTContentCalculator::~TTContentCalculator()
{
    _figures.clear();
    _figuresWeight.clear();
    _fixedFigures.clear();
}

bool TTContentCalculator::init(void)
{
    _figuresWeight.clear();
    _figures.clear();
    _fixedFigures.clear();
    return true;
}


//inicializa sistema de calculo de pesos de figuras
void TTContentCalculator::init(std::vector<int> figuresWeight)
{
    _fixedFigures.clear();
    this->_figuresWeight = figuresWeight;

    generateMoreFigures();
}



//devuelve la siguiente figura en funcion del tipo de algoritmo definido
int TTContentCalculator::nextFigure(int fixedFigure)
{
    if (fixedFigure!=-1)
    {
        _fixedFigures[fixedFigure]++;
        return fixedFigure;
    }
    if (TYPE_RANDOM)
    {
        return nextFigureRandom();
    }
    else
    {
        return nextFigurePseudoRandom();
    }
}


//devolver la siguiente figura fullrandom
int TTContentCalculator::nextFigureRandom()
{
    // Calculamos el sumatorio de probabilidades
    int _weightSum = 0;
    for(int i = 0; i < _figuresWeight.size(); i++)
    {
        _weightSum += _figuresWeight[i];
    }
    int randWeight = 1+(rand()%_weightSum);
    int weight = 0;
    for(int i = 0; i < _figuresWeight.size(); i++)
    {
        if(i == _figuresWeight.size()-1)
        {
            return i;
        }
        weight += _figuresWeight[i];
        if(weight >= randWeight)
        {
            return i;
        }
    }
    return 0;
}

//evolver la siguiente figura de manera pseudo random
//se tienen en cuenta los pesos, y se asegura
//que salgan tantas figuras como esté definido en el peso
int TTContentCalculator::nextFigurePseudoRandom()
{
    int _randomIdx = rand()%_figures.size();
    int toReturn = _figures[_randomIdx];
    _figures.erase(_figures.begin()+_randomIdx);
    if (_figures.size() == 0)
    {
        generateMoreFigures();
    }
    //control de fixedfigures
    int fixedNum = _fixedFigures[toReturn]--;
    if (fixedNum>=0)
    {
        toReturn = nextFigurePseudoRandom();
    
    } else {
        _fixedFigures[toReturn]=0;
    }
    return toReturn;
    
}

void TTContentCalculator::generateMoreFigures()
{
    this->_figures.clear();
    
    const int factor=3;
    //rellenar el vector de figuras grande teniendo en cuenta los pesos
    for(int i = 0; i < _figuresWeight.size(); i++)
    {
        int total = factor * _figuresWeight[i];
        for(int j=0; j<total; j++)
        {
            _figures.push_back(i);
        }
    }
    //mezclar el vector
    std::srand((unsigned int)time(NULL));
    std::random_shuffle(_figures.begin(), _figures.end(), RNG());
}
