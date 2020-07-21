#ifndef PARAMETERS_H
#define PARAMETERS_H


class Parameters
{
private:
    const int  MAX_TIME			      = 60;  // limite de tempo em minutos
    const int  TIME_INTERVAL		  = 1;   // discretizacao: MAX_TIME/TIME_INTERVAL => numero de pontos de posicoes para cada coleta e entrega
    const int  MATRIX_NUMBER_COLUMNS  = 100;  // numero de colunas da matriz que representa o mapa
    const int  MATRIX_NUMBER_LINES    = 100;  // numero de linhas da matriz que representa o mapa
    const int  BATTERY_RECHARGE       = 10; // valor da recarga da bateria do drone
    const int  MAX_BATTERY_CHARGE     = 20; // valor da carga da bateria do drone cheia
          int  initial_battery_charge = 10; // nivel de bateria do drone
    const int  CONSUMPTION_BATTERY    = 2;   // consumo de bateria do drone por intervalo de tempo: a cada 1 TIME_INTERVAL o nivel de bateria cai de 1 CONSUMPTION_BATTERY
    const int  QTY_POINTS[4]           = { 0, 50, 50, 50 }; // qtd de pontos de parada para cada tipo: 0 - sem valor; 1 - DELIVER; 2 - COLLECT; 3 - CHARGE
    const int  NUMBER_ITERATIONS      = 100;

public:
    Parameters();
//    Parameters(const int max_time,
//               const int time_interval,
//               const int matrix_number_line,
//               const int battery_recharge,
//               const int max_battery_charge,
//                     int initial_battery_charge,
//               const int consumption_battery,
//               const int qty_points[],
//               const int number_iterations);
};

#endif // PARAMETERS_H
