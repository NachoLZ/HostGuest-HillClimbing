#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <string>
#define config "config1" // <-- Cambiar acá sólo el número de archivo configX.txt a usar

using namespace std;
int T, H, G, boats;
vector<int> constraints;

/*
 Input: N/A
 Output: N/A
 Lee los archivos para generar el vector constraints, el cual incluye la capacidad y tamano de
 tripulación de cada Host y Guest, los Guest tienen capacidad -1.
 */
void Read(){
    size_t pos, pos2;
    int aux, marca = 1, i = 0;
    string boat, linea, linea2, a, b, delim = ";", delim2 = " ";

    ifstream Instancia;
    Instancia.open("PPP.txt");
    if(Instancia.fail()) // Chequea si se abrio el archivo
    {
        cout << "Error al leer PPP.txt" << endl;
    }
    ifstream Config;
    Config.open((string)config + ".txt");
    if(Config.fail()) // Chequea si se abrio el archivo
    {
        cout << "Error al leer configx.txt" << endl;
    }

    Instancia >> boats;
    Instancia >> T;

    constraints =  vector<int>(boats*2);

    Instancia >> linea;

    while ((pos = linea.find(delim)) != string::npos){

        boat = linea.substr(0, linea.find(delim));
        pos2 = boat.find(',');
        constraints[i] = stoi(boat.substr(0, boat.find(',')));

        boat.erase(0, pos2 + 1);
        constraints[i+1]= stoi(boat);

        linea.erase(0, pos + delim.length());

        i += 2;


    }

    boat = linea.substr(0, linea.find(delim));
    pos2 = boat.find(',');
    constraints[i] = stoi(boat.substr(0, boat.find(',')));

    boat.erase(0, pos2 + 1);
    constraints[i+1]= stoi(boat);

    linea.erase(0, pos + delim.length());

    i += 2;


    while (Config >> linea2){

        aux = stoi(linea2);
        for(;marca<aux;marca++){
            constraints[(marca-1)*2] = -1;

        }
        marca = aux+1;
        H++;
    }
    for(; marca<=boats; marca++){
        constraints[(marca-1)*2] = -1;
    }
    Instancia.close();
    Config.close();
    /*
    for (vector<int>::const_iterator j = constraints.begin(); j != constraints.end(); ++j)
        cout << *j << ' ';*/

}
/*
Input: N/A
Output: Matriz de 0s de tamano G*T que será trabajada con Greedy para obtener la solución inicial
*/
vector<vector<int>> build_matrix(){
    vector<vector<int>> sol(G);
    for (int i=0; i < G; i++)
        sol[i] = vector<int>(T);

    for(int i=0; i < G; i++){
        for(int j=0; j < T; j++){
            sol[i][j] = 0;
        }
    }
    return sol;
}

/*
Input: int numero -> numero de boat (host o guest) a mapear.
       bool host -> true si se quiere mapear un host, false si se quiere mapear un guest.
Output: Numero de anfitrion o guest mapeado.

Esto es debido a que por ejemeplo, el host 13 corresponde al host número 16, por lo que arregla
los números para el último especialmente.
*/
int fix(int numero, bool host){

    int i, cont = 0;

    if(host){
        for(i = 0; cont < numero; i += 2){
            if(constraints[i] != -1){
                cont ++;
            }
        }
    }

    if(!host){
        for(i = 0; cont < numero; i += 2){
            if(constraints[i] == -1){
                cont ++;
            }
        }
    }

    return (i/2 - 1);
}

/*
Input: matriz -> Matriz de tamano G*T que representa una solución a evaluar.
Output: int Cost -> Costo total de la solución evaluada. Este Cost se calcula de acuerdo a
        la suma de penalizaciones obtenidas por las restricciones que se violan.
*/

int Cost(vector<vector<int>> matriz){
    int costo = 0, i, j, aux, indice, x;
    vector<int> vec_costos(G*(G-1)/2);
    vector<int> copy(boats*2);

    //Se anade costo por la restriccion DIFF
    for(i = 0; i < G; i++){
        for(j = 0; j < T; j++){
            for(x = j+1; x < T; x++){
                if((matriz[i][x] == matriz[i][j]) && matriz[i][x] != 0){
                    costo += 4;
                }
            }
        }
    }
    //Se anade costo por la restriccion ONCE

    for(aux = 0; aux < G*(G-1)/2; aux++){
        vec_costos[aux] = 0;
    }

    for(j = 0; j < T; j++){
        indice = 0;
        for(i = 0; i < G; i++){
            for(x = i + 1; x < G; x++){
                if((matriz[i][j] == matriz[x][j]) && matriz[i][j] != 0){
                    vec_costos[indice] = vec_costos[indice] + 1;
                }
                indice++;
            }
        }
    }

    for(aux = 0; aux < G*(G-1)/2; aux++){
        if(vec_costos[aux] > 0){
            costo += 2*(vec_costos[aux]-1);
        }
    }

    //Se anade costo por la restriccion CAPA

    for(j = 0; j < T; j++){

        for(i = 0; i < boats*2; i+=2){
            if(constraints[i] > 0){
                copy[i] = constraints[i]-constraints[i+1];
                copy[i+1] = constraints[i+1];
            }
            else{
                copy[i] = 10;
                copy[i+1] = constraints[i+1];
            }
        }
        for(i = 0; i < G; i++){
            int host, guest;
            host = matriz[i][j];
            guest= i;
            if(host != 0){
                int indice_host = fix(host, true) + 1;
                int indice_guest = fix(guest + 1, false) + 1;
                copy[(indice_host-1)*2] =  copy[(indice_host-1)*2] - copy[(indice_guest-1)*2+1];
            }

        }
        for(i = 0; i < 2*(G+T); i += 2){
            if(copy[i] < 0){
                costo += (int)(4*(1+(float)(-copy[i]-1)/(float)4));
            }
        }
    }
    return costo;
}

/*
Input: N/A
Output: Matriz sol -> Solución inicial encontrada por Greedy
*/

vector<vector<int>> Greedy(){
    vector<vector<int>> s = build_matrix();
    int cost, t, g, h, indice;
    for(t = 0; t < T; t++){
        for(g = 0; g < G; g++){
            int final_host[] = {0, 0, 0};
            int arr[] = {100000, 100000, 100000};
            for(h = 1; h <= H; h++){
                s[g][t] = h;
                cost = Cost(s);
                if(cost < arr[0]){
                    final_host[0] = h;
                    arr[0] = cost;
                }
                else if (cost < arr[1])
                {
                    final_host[1] = h;
                    arr[1] = cost;
                }
                else if (cost < arr[2])
                {
                    final_host[2] = h;
                    arr[2] = cost;
                }
            }
            indice = rand() % 3;
            s[g][t] = final_host[indice];
        }
    }
    return s;
}


/*
Input: Matriz marcas -> Matriz de puros 0s, con 1s donde ya se han hecho movimientos en la solución que se está tratando.
       int col -> Indica en que columna se va a marcar.
       int row -> Indica en que fila se va a marcar.
Output: Matriz marcas -> Matriz con una nueva marca indicada por la fila "row" y la columna "col"
*/
vector<vector<int>> marcar(vector<vector<int>>marcas, int row, int col){
    marcas[row][col] = 1;
    return marcas;
}


/*
Input: Matriz sol -> Matriz de la solución inicial entregada por Greedy.
Output: Matriz marcas -> Matriz de la solución con Hill Climbing Alguna Mejora aplicado.
*/
vector<vector<int>> HillClimbing(vector<vector<int>>sol){
    vector<vector<int>> marcas(G);
    vector<vector<int>> sol_mutada;
    int i, j;

    for (i = 0; i < G; i++){
        marcas[i] = vector<int>(T, 0);
    }

    int prev_cost = Cost(sol);

    for (i = 0; i < 500; i++){
        int row = (rand() % G);
        int col = (rand() % T);

        while (marcas[row][col] == 1){  //Para asegurar que no se repiten movimientos
            row = (rand() % G);
            col = (rand() % T);
        }

        sol_mutada = sol;
        int elegido = sol_mutada[row][col];


        sol_mutada[row][col] = (rand() % H) + 1;

        while (elegido == sol_mutada[row][col]){  //Para asegurar que el rand no asigno el numero que ya habia
            sol_mutada[row][col] = (rand() % H) + 1;
        }

        marcas = marcar(marcas, row, col);

        int new_cost = Cost(sol_mutada);
        if (new_cost < prev_cost){
            sol = sol_mutada;
            prev_cost = Cost(sol);
            for (j = 0; j<G; j++){
                marcas[j] = vector<int>(T, 0);
            }
        }
    }

    return sol;
}

/*
Input: Matriz sol_hc -> Solución con Hill Climbing Alguna Mejora aplicado.
Output: Matriz sol_hc -> Solución anterior cambiada de vecindario.
 */
vector<vector<int>> Restart(vector<vector<int>> sol_hc){
    vector<vector<int>> marcas(G);

    for (int i = 0; i < G; i++){
        marcas[i] = vector<int>(T, 0);
    }

    for (int i=0; i<(G*T); i++){
        int row = (rand() % G);
        int col = (rand() % T);


        sol_hc[row][col] = (rand() % H) + 1;

        marcas = marcar(marcas, row, col);

    }

    return sol_hc;
}

/*
Input: Matriz sol_hc -> Solucion con Hill Climbing - AM, para adaptarla al archivo de salida.
Output: int 0
 */
int Output(vector<vector<int>> sol_hc){
    vector<int> hosts(H, 0);
    int t, h, g, crewsize = 0, hostcont, j = H;
    string guests = "";
    ofstream salida;
    salida.open((string)config + ".out");

    for (t = 1; t <= T; t++){
        salida << endl << "t=" << t << endl;
        salida << "----------" << endl;
        for (h = 1; h <= H; h++){
            for (g = 0; g < G; g++){
                if (sol_hc[g][t-1] == h){
                   guests += to_string(fix(g+1, false) +  1)+", ";
                   crewsize += constraints[(2*fix(g+1, false))+1];
                    if (crewsize == 0){
                        hosts[h-1] = 1;

                    }else{
                        hosts[h-1] = 0;
                    }
                }
            }
            salida << fix(h, true) << " -> " << guests << "[" << to_string(crewsize) << "/" << to_string(constraints[2*fix(h, true)]) << "]" << endl;

            guests = "";
            crewsize = 0;
        }
        hostcont = H;
        for (h = 0; h < H; h++){
            if (hosts[h] == 1){
                hostcont--;
            }
        }
        if (j < hostcont){
            j = hostcont;
        }
    }

    salida << endl << "La cantidad de yates anfitriones óptimos es: " << to_string(j) << endl;

    salida.close();
    return 0;
}

int main(){
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    int i, j;
    //Lectura de archivos

    Read();

    G = boats-H;

    //Se aplica Greedy para construir una solución inicial
    vector<vector<int>> sol = Greedy();

    cout << "Guests (G): " << G << endl;
    cout << "Hosts (H): " << H << endl;
    cout << "Periodos (T): " << T << endl << endl;

    //Se imprime la solucion inicial con Greedy
    for(i = 0; i < G; i++){
        for(j = 0;j < T; j++){
            cout << fix(sol[i][j],true) + 1 << " \n"[j == T-1];
        }
        cout << endl;
    }

    //Costo total de la sol. inicial
    cout << "Costo inicial con Greedy: " << Cost(sol) << endl << endl;


    //Se le aplica Hill Climbing (Alguna Mejora) a la solución actual.
    sol = HillClimbing(sol);
    Output(sol);
    for(i = 0; i < G; i++){
        for(j = 0; j < T; j++){
            cout << fix(sol[i][j],true) + 1 << " \n"[j == T-1];
        }
        cout << endl;
    }

    cout << "Costo con Greedy + HC: " << Cost(sol) << endl << endl;

    //Ahora se hace un Restart y otro HC (AM)
    sol = Restart(sol);
    sol = HillClimbing(sol);

    for(i = 0; i < G; i++){
        for(j = 0; j < T; j++){
            cout << fix(sol[i][j],true) + 1 << " \n"[j == T-1];
        }
        cout << endl;
    }

    cout << "Costo Total de Greedy + HC (AM) + R: " << Cost(sol) << endl << endl;
    ofstream salida;
    salida.open((string)config + ".out", ios::app);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    salida << endl << "Tiempo de Ejecucion (segundos) = " <<  (chrono::duration_cast<chrono::microseconds>(end - begin).count()) /1000000.0  <<endl;
    salida.close();

    return 0;
}