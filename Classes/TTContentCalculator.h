//
//  TTContentCalculator
//  animalplanet
//
//  Created by Albert on 09/11/14.
//
//

#ifndef __animalplanet__TTContentCalculator__
#define __animalplanet__TTContentCalculator__

#include "thegame.h"

class TTContentCalculator
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTContentCalculator* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTContentCalculator(){}
    
public:
    
    virtual ~TTContentCalculator();
    bool init();
    
    //inicializa sistema de calculo de pesos de figuras
    void init(std::vector<int> figuresWeight);
    
    //devuelve la siguiente figura
    int nextFigure(int fixedFigure=-1);
    
private:
    
    std::vector<int> _figuresWeight;
    std::vector<int> _figures;
    
    std::map<int,int> _fixedFigures;
    
private:

    int nextFigureRandom();
    
    int nextFigurePseudoRandom();
    
    void generateMoreFigures();
};

#endif /* defined(__animalplanet__TTContentCalculator__) */
