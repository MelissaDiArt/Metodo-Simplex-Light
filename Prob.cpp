#include "prob.h"
#include <iomanip>

PROBLEM::PROBLEM(char nombrefichero[85]){

unsigned i, j;
double dummy;
ifstream textfile;

textfile.open(nombrefichero);
 if (textfile.is_open()) {
  textfile >> (char *) clase >> (unsigned &) n >> (unsigned &) m;
  A.resize(m); // m filas
  
  for (i=0;i<n; i++) {  //leemos la fila de los n costes
    textfile >> (double &) dummy;
    c.push_back(dummy);
  }
 // si el problema es de máximo cambiamos de signo los costes
  if (strncmp(clase,"max",3) == 0)
    for(i=0;i<n;i++) c[i] *= -1.0;   
    // extendemos el vector c a tamaño n + nh
    nh = m;
    for(i=0;i<nh;i++) c.push_back(0.0);  // leemos las m restricciones y asignamos A, b, ivb
      for (i=0;i<m;i++) {
        for (j=0;j<n;j++) {
          textfile >> (double &) dummy;
          A[i].push_back(dummy);
        }
        textfile >> (double &) dummy;
        b.push_back(dummy);
        if (dummy < CERONEG){
          cout << "Lado derecho negativo, Modifique problema" << endl;
          exit(0);
        }
        A[i].resize(n+nh);  // extendemos la fila a tamaño n+nh
        A[i][n+i] = 1.0;    // ponemos un 1 en la columna n+i fila i correspondiente a la v. de holgura de la fila i
        ivb.push_back(n+i); // la v. básica asociada a la fila i es n+i
      }
      textfile.close();
 }
}

void PROBLEM::Volcar_problema(void)
{
  
 char res[100];
 
 cout << endl << "Z= " << clase;
 
 
  for (int i=0; i <n; i++){
    if (strncmp(clase,"max",3) == 0){ 
     
      if ((-1*c[i])>CERONEG){
        sprintf(res," +%4.2lf",(-1*c[i]));      
      }else sprintf(res," %4.2lf",(-1*c[i]));      
   
   cout << res << "x" << i+1;
   
  }else {
   if (c[i]>CERONEG){
     sprintf(res," +%4.2lf",c[i]);
     }else sprintf(res," %4.2lf",c[i]);
    
    cout << res << "x" << i+1;
    
   }
  }
 cout << endl << "Sujeto a: " << endl;
 
 for (int i=0; i<m; i++){
   for (int j=0; j<n; j++){
     if (A[i][j]>CERONEG)
     sprintf(res," +%4.2lf",A[i][j]);
     else sprintf(res," %4.2lf",A[i][j]);
     
   cout << res << "x" << i+1;}
   
   if (b[i]>=CERONEG){
      sprintf(res,"+%4.2lf",b[i]);
    }else{
      sprintf(res,"%4.2lf",b[i]);
    }
   cout << " <=" << res << endl;
 }
}
 
void PROBLEM::volcar_tabla(void)
{

 cout << endl << endl << "---------------------------------- SIMPLEX PRIMAL ----------------------------------" << endl << endl;
 
 cout << "    ";
 for (int i=1; i<=n; i++)
   cout << "     x" << i << " ";
 for (int j=1; j<=m; j++)
   cout << "     h" << j << " ";
 cout << "     b " << endl;
 
 for (int i=1; i<=n+m+2; i++)
   cout << "--------";
 cout << endl;
 
 char res[100];

 for (int i=0; i<m; i++){
    if(ivb[i]<n)
      cout<<"x"<<ivb[i]+1<<" |";
    else{
	cout<<"h"<<ivb[i]-n+1<<" |";}
    for (int j=0; j<n+nh; j++){
      if(A[i][j]>CERONEG)
	sprintf(res," %+6.1lf ",A[i][j]);
      else{
	sprintf(res," %6.1lf ",A[i][j]);
      } 
    cout<<res;}
    
    if(b[i]>=CERONEG)
      sprintf(res," %+6.1lf",b[i]);
    else{
      sprintf(res,"  %6.1lf",b[i]);
    }  
    cout << res << endl;
  }
 
 for (int i=1; i<=n+m+2; i++)
   cout << "--------";
 cout << endl;
 
 cout << "-Z ";
 for (int i=0; i<n+nh; i++){
   if(c[i]>=CERONEG){
       sprintf(res,"  %+6.1lf",c[i]);
   }else{
      sprintf(res,"  %6.1lf",c[i]);
    }  
    cout << res;}

 if(Vo>CERONEG){
       sprintf(res,"  %+6.1lf",Vo);
   }else{
      sprintf(res,"  %6.1lf",Vo);
    }  
 cout << res << endl;
}

unsigned PROBLEM::entrante()
{
  int s=UERROR;
  double min= CERONEG;

  for (int i=0; i<n+nh;i++){
    if(c[i]<min){
      min=c[i];
      s=i;
    }
  }
 
  return(s);
}
unsigned PROBLEM::saliente (unsigned s)
{
  double min= UERROR;
  int r=UERROR;

  for (int i=0; i<m; i++){
    if ((min>(b[i]/A[i][s]))&&(A[i][s]>CEROPOS)){
      min=b[i]/A[i][s];
      r=i;
    }
  }
  return(r);
}

void PROBLEM::actualizar_valores(unsigned s, unsigned r){    //Actualizar valores de las tablas
   
    double temp;
 
    ivb[r] = s; //actualización de la base
    temp = A[r][s];
    for (int i=0;i<n+nh;i++)
      A[r][i] /=temp;
    b[r] /= temp;
 
    for (int i=0; i<m;i++)
      if ((i!=r)&&(A[i][s]!=0)){  //Para todo i=1 hasta m con i<>r y A[i][s] <>0
    temp = A[i][s];
        for (int j=0;j<n+nh;j++)
      A[i][j] -= A[r][j] * temp;  //la fila entera de i
    b[i] -= b[r] * temp;
      }
     
      Vo -= b[r] * c[s]; //actualizamos Vo
     
      for (int i=0;i<n+nh;i++)
    if (i!=s)  //Para todo i<>s
      c[i] -= c[s] * A[r][i];
    c[s] = 0.0;
}

void PROBLEM::Simplex_Light(){
  
  unsigned s,r;
  
  while ((s = entrante())!= UERROR) {
    volcar_tabla();
    r = saliente(s);
    if (r != UERROR)
      actualizar_valores(s,r);
    else{ cout << endl << endl << "PROBLEMA NO ACOTADO" << endl << endl;
      exit(0);}
  }
  volcar_tabla();
  mostrar_solucion();
}

void PROBLEM::mostrar_solucion(void) 
{
  
char res [100];
cout << endl << "****************************************************************************************" << endl << "                                     SOLUCION                                       " << endl << endl;

  for(int i=0;i<m;i++){
    if(ivb[i]<n){
      sprintf(res," +%4.3lf ",b[i]);
      cout << " x" << ivb[i]+1 << "=" << res;
    }else{
      if(ivb[i]<n+nh)
	sprintf(res," +%4.3lf ",b[i]);
	cout << " h" << ivb[i]-n+1 << "=" << res;
    }
  }
  cout << " Z= ";
  
  if (strncmp(clase,"max",3) == 0){
    if (Vo > 0){
      sprintf(res,"+%4.3lf ",Vo);
    }else{
      sprintf(res,"%4.3lf ",Vo);
    }
    cout << res;
    }else{
      if ((Vo*-1) >0){
	sprintf(res,"+%4.3lf ",Vo*-1);
      }else{
	sprintf(res,"%4.3lf ",Vo*-1);
      }
    cout << res;
    }

  cout << endl << "****************************************************************************************" << endl;
}
