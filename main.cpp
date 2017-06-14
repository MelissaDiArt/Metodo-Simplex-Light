#include "prob.h"

int main()
{
  char nombre[85];
  cout << "Introduzca el nombre del archivo: ";
  cin >> (char*) nombre;
  PROBLEM p(nombre);
  p.Volcar_problema();
  p.Simplex_Light();

}
