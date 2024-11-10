# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta



//Encargado de ejecutar la acción del jugador y de interactuar con el tablero. NO SE MODIFICA
bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

//Implementar el proceso de decisión. EL valor del mejor movimiento elegido se almacena en las variables pasadas como referencia. 
void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTestUno);
            break;

        case 2:
            thinkAleatorio(c_piece, id_piece, dice);
        break;

        case 3:
            thinkAleatorioMasInteligente(c_piece, id_piece, dice);
        break;

        case 4:
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
        break;

        case 5:
            thinkMejorOpcion(c_piece, id_piece, dice);
        break;
    }
    
}

//HEURÍSTICA DE PRUEBA (MOVIMIENTOS ALEATORIOS)
double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

//HEURÍSTICA PARA LA ENTREGA: 
double AIPlayer::ValoracionTestUno(const Parchis &estado, int jugador){
    
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador){
        return gana;
    }else if (ganador == oponente){
        return pierde;
    }else{
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        //Fuerza del dado especial del jugador y el oponente
        int pb_jugador = estado.getPower(jugador);
        int pb_oponente = estado.getPower(oponente);

        
        //Piezas destrozadas en el último movimiento: 
        vector<pair<color, int>> destroyed_by_lastMove = estado.piecesDestroyedLastMove();


        // Recorro todas las fichas de mi jugador/////////////////////////////////////////////////////////////////////////////////////////////////////
        int puntuacion_jugador = 0;
        
        for (int i = 0; i < my_colors.size(); i++){
            
            color c = my_colors[i];
            
            for (int j = 0; j < num_pieces; j++){
                
                //Puntuación según la posición de las fichas:
                if (estado.isSafePiece(c, j)){
                    puntuacion_jugador+=10;
                }else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_jugador += 50;
                 }else if (estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_jugador += 15;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_jugador -=10;
                }

                //Puntuación según movimientos realizados: 
                if(estado.isEatingMove()){
                    if(estado.eatenPiece().first == my_colors[0] || estado.eatenPiece().first == my_colors[1] || estado.eatenPiece().first == my_colors[2]) puntuacion_jugador-=20;
                    else puntuacion_jugador += 30;
                } else if(estado.isGoalMove()){
                            puntuacion_jugador+=40;
                } else if (estado.goalBounce()) puntuacion_jugador -=10;


                //Puntuación según la distancia a la meta: 
                puntuacion_jugador += (72-estado.distanceToGoal(c,j));


                //Puntuación según las fichas destrozadas en el último movimiento: 
                for (int i=0;i<destroyed_by_lastMove.size();i++){
                    if(destroyed_by_lastMove[i].first == my_colors[0] or destroyed_by_lastMove[i].first == my_colors[1]){
                        puntuacion_jugador-=30;
                    }else{
                        puntuacion_jugador+=30;
                    }
                }

                //Puntuación según la fuerza de la Power Bar: 
                if(pb_jugador >= 0 && pb_jugador < 50){
                    puntuacion_jugador += 10;
                }else if((pb_jugador >= 50 && pb_jugador < 60) || (pb_jugador >= 70 && pb_jugador < 75)){
                    puntuacion_jugador += 20;
                }else if(pb_jugador >= 60 && pb_jugador < 65){
                    puntuacion_jugador -= 10;
                }else if (pb_jugador >= 65 && pb_jugador < 70){
                    puntuacion_jugador += 20;
                }else if (pb_jugador >= 75 && pb_jugador < 80){
                    puntuacion_jugador += 30;
                }else if (pb_jugador >= 80 && pb_jugador < 85){
                    puntuacion_jugador -= 10;
                }else if (pb_jugador >= 85 && pb_jugador < 90){
                    puntuacion_jugador += 30;
                }else if (pb_jugador >= 90 && pb_jugador < 95){
                    puntuacion_jugador -= 10;
                }else if (pb_jugador >= 95 && pb_jugador < 100){
                    puntuacion_jugador += 40;
                }else if (pb_jugador >= 100){
                    puntuacion_jugador -= 20;
                }
            }
        }

        // Recorro todas las fichas del oponente////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int puntuacion_oponente = 0;
        
        for (int i = 0; i < op_colors.size(); i++){

            color c = op_colors[i];

            for (int j = 0; j < num_pieces; j++){
                
                //Puntuación según la posición de las fichas:
                if (estado.isSafePiece(c, j)){
                    puntuacion_oponente+=10;
                }else if (estado.getBoard().getPiece(c, j).get_box().type == goal){
                    puntuacion_oponente += 50;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == final_queue){
                    puntuacion_oponente += 15;
                }else if(estado.getBoard().getPiece(c, j).get_box().type == home){
                    puntuacion_oponente -=10;
                }

                //Puntuación según movimientos realizados: 
                 if(estado.isEatingMove()){
                    if(estado.eatenPiece().first == op_colors[0] || estado.eatenPiece().first == op_colors[1] || estado.eatenPiece().first == op_colors[2]) puntuacion_oponente-=20;
                    else puntuacion_oponente += 30;
                } else if(estado.isGoalMove()){
                            puntuacion_oponente+=40;
                } else if (estado.goalBounce()) puntuacion_oponente -=10;


                //Puntuación según la distancia a la meta: 
                puntuacion_oponente += (72-estado.distanceToGoal(c,j));


                //Puntuación según las fichas destrozadas en el último movimiento: 
                for (int i=0;i<destroyed_by_lastMove.size();i++){
                    if(destroyed_by_lastMove[i].first == op_colors[0] or destroyed_by_lastMove[i].first == op_colors[1]){
                        puntuacion_oponente-30;
                    }else{
                        puntuacion_oponente+=30;
                    }
                }

                //Puntuación según la fuerza de la Power Bar: 
                if(pb_oponente >= 0 && pb_oponente < 50){
                    puntuacion_oponente += 10;
                }else if((pb_oponente >= 50 && pb_oponente < 60) || (pb_oponente >= 70 && pb_oponente < 75)){
                    puntuacion_oponente += 20;
                }else if(pb_oponente >= 60 && pb_oponente < 65){
                    puntuacion_oponente -= 10;
                }else if (pb_oponente >= 65 && pb_oponente < 70){
                    puntuacion_oponente += 20;
                }else if (pb_oponente >= 75 && pb_oponente < 80){
                    puntuacion_oponente += 30;
                }else if (pb_oponente >= 80 && pb_oponente < 85){
                    puntuacion_oponente -= 10;
                }else if (pb_oponente >= 85 && pb_oponente < 90){
                    puntuacion_oponente += 30;
                }else if (pb_oponente >= 90 && pb_oponente < 95){
                    puntuacion_oponente -= 10;
                }else if (pb_oponente >= 95 && pb_oponente < 100){
                    puntuacion_oponente += 40;
                }else if (pb_oponente >= 100){
                    puntuacion_oponente -= 20;
                }

            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

//PODA: 
double AIPlayer::PodaAlfaBeta(const Parchis &estado, int jugador, int prof, int prof_max, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristica)(const Parchis &, int)) const {
    double valor_hijo;
    bool nodoMAX = (estado.getCurrentPlayerId() == jugador); // Determinar si es nodo MAX o MIN

    // Verificar si hemos llegado al final o no
    if (prof == prof_max || estado.gameOver()) {
        return heuristica(estado, jugador);
    }

    // Generar hijos y iterar
    ParchisBros hijos = estado.getChildren();

    // Recorrer hijos generados
    for (auto it = hijos.begin(); it != hijos.end(); ++it) {
        valor_hijo = PodaAlfaBeta(*it, jugador, prof + 1, prof_max, c_piece, id_piece, dice, alpha, beta, heuristica);

        // Si es nodo MAX
        if (nodoMAX) {
            if (valor_hijo > alpha) {
                alpha = valor_hijo;
                // Actualizar jugada si es la raíz
                if (prof == 0) {
                    id_piece = it.getMovedPieceId();
                    c_piece = it.getMovedColor();
                    dice = it.getMovedDiceValue();
                }
            }
            if (alpha >= beta) { // Podar
                return alpha;
            }
        } 
        // Si es nodo MIN
        else {
            if (valor_hijo < beta) {
                beta = valor_hijo;
            }
            if (beta <= alpha) { // Podar
                return beta;
            }
        }
    }

    // Retornar el valor correspondiente
    if (nodoMAX) {
        return alpha;
    } else {
        return beta;
    }
}


void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const{

    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }

    
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
    // Llamada a la función para la poda (los parámetros son solo una sugerencia, se pueden modificar).
    valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    // ----------------------------------------------------------------- //

    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            //valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;
        case 2:
            //valor = PodaAlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const{
    
    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    
    //Miro que dados tienen fichas que se puedan mover
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++){

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices[i]);
        
        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0) current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        
    }

    //Si no tengo ningun dado que pueda mover fichas, paso turno con un dado al azar.
    if (current_dices_que_pueden_mover_ficha.size() == 0){
        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    
    }else{// En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.

        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];       

        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);

        // Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const{
    
    // Elijo el dado haciendo lo mismo que el jugador anterior. Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);

    // Ahora, en vez de mover una ficha al azar, voy a mover la que esté más adelantada
    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;

    for (int i = 0; i < current_pieces.size(); i++){
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta)
        {
        min_distancia_meta = distancia_meta;
        id_ficha_mas_adelantada = id;
        col_ficha_mas_adelantada = col;
        }
    }

    // Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1){
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
   
    }else{ // En caso contrario, moveré la ficha más adelantada.
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const{
    //Voy a recorrer todas las opciones posibles y ver si en algún caso se cumple que me coma una ficha
    //o llegue a la meta, o gane la partida. Si se cumple, me quedo con la acción que me lleva a ese hijo,
    //si no, me quedo con la acción que suponga un mayor incremento de energía.

    ParchisBros hijos = actual->getChildren(); //Para el estado (tablero) actual genero todos los hijos (opciones posibles)
    
    bool me_quedo_con_esta_accion = false;

    int current_power = actual->getPowerBar(this->jugador).getPower();
    int max_power = -101; // Máxima ganancia de energía

    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and !me_quedo_con_esta_accion; ++it){
        
        Parchis siguiente_hijo = *it;
        
        // Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa acción.
        // Termino el bucle en este caso.
        if (siguiente_hijo.isEatingMove() or siguiente_hijo.isGoalMove() or (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador)){
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }else{// En caso contrario, me voy quedando con el que me dé más energía.
            int new_power = siguiente_hijo.getPower(this->jugador);
            if(new_power - current_power > max_power){
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
            max_power = new_power - current_power;
            }
        }
    }
}