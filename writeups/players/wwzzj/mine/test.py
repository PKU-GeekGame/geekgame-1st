#hecho por Lina M. Varon y Juan Pablo Rodr�guez
import random
from random import randint

class casilla:
  def __init__(self,proba,valor,descubierto):
    self.proba = proba             #probabilidad que sea mina
    self.valor = valor             #Valor de la casilla ��9 es mina!!
    self.estaDescubierto = descubierto      #si la casilla fue ya destapada por el jugador
  #enddef
#endcasilla

def setupTablero(h,w,n):

  posMinas = random.sample(range(h*w),n)
  posMinas.sort()
  print (posMinas)
  tablero = []

  cont_agregar = 0;
  cont_tablero = -1;

  for i in range(h):
    tablero.append([])
    for j in range(w):
      cont_tablero += 1
     # print (cont_tablero,cont_agregar,posMinas,posMinas[cont_agregar])
      if(cont_agregar<=len(posMinas) -1 and cont_tablero == posMinas[cont_agregar]):
        cont_agregar += 1
        tablero[i].append(casilla(n/(h*w),9,False))
      else:
        tablero[i].append(casilla(n/(h*w),0,False))      
    #endfor
  #endfor

  for i in range(h):
    for j in range(w):

      contMinas = 0

      p_si = [i-1,j-1]            #punto superior izquierda
      p_sc = [i-1,j]              #punto superior centro
      p_sd = [i-1,j+1]            #punto superior derecho

      p_ci = [i,j-1]              #punto centro izquierda
      #p_cc = [i,j]               #punto centro centro
      p_cd = [i,j+1]              #punto centro derecho

      p_ii = [i+1,j-1]            #punto inferior izquierda
      p_ic = [i+1,j]              #punto inferior centro
      p_id = [i+1,j+1]            #punto inferior derecho

      
      if(verificar_dentro(h,w,p_si) and tablero[i][j].valor!=9 and tablero[p_si[0]][p_si[1]].valor==9):
        contMinas += 1
      #end if
      if(verificar_dentro(h,w,p_sc) and tablero[i][j].valor!=9 and tablero[p_sc[0]][p_sc[1]].valor==9):
        contMinas += 1
      #end if
      if(verificar_dentro(h,w,p_sd) and tablero[i][j].valor!=9 and tablero[p_sd[0]][p_sd[1]].valor==9):
        contMinas += 1
      #end if

      if(verificar_dentro(h,w,p_ci) and tablero[i][j].valor!=9 and tablero[p_ci[0]][p_ci[1]].valor==9):
        contMinas += 1
      #end if
      if(verificar_dentro(h,w,p_cd) and tablero[i][j].valor!=9 and tablero[p_cd[0]][p_cd[1]].valor==9):
        contMinas += 1
      #end if

      if(verificar_dentro(h,w,p_ii) and tablero[i][j].valor!=9 and tablero[p_ii[0]][p_ii[1]].valor==9):
        contMinas += 1
      #end if
      if(verificar_dentro(h,w,p_ic) and tablero[i][j].valor!=9 and tablero[p_ic[0]][p_ic[1]].valor==9):
        contMinas += 1
      #end if
      if(verificar_dentro(h,w,p_id) and tablero[i][j].valor!=9 and tablero[p_id[0]][p_id[1]].valor==9):
        contMinas += 1
      #end if
      if(tablero[i][j].valor==0):
        tablero[i][j].valor = contMinas
      #endif
    #endfor
  #endfor
  imprimirTablero(tablero,"valor")
  return tablero
#enddef

def verificar_dentro(h,w,p):
  i=int(p[0])
  j=int(p[1])
  if(i<0 or i>=h or j<0 or j>=w):
    return False
  #end if
  return True
#end def

#Tablero del usuario
def imprimirTablero(tablero,att):
  for i in tablero:
    for j in i:
      if att == "valor":
        print (j.valor,"\t",end="")
      elif att == "estaDescubierto" :
        print (j.estaDescubierto,"\t",end="")
      elif att == "juego":
        if(j.estaDescubierto):
          print (j.valor,"\t",end="")
        else:
          print ("-\t",end="")
    #endfor
    print("")
  #endfor
#enddef

#Coordenas usuario
def coodUsuario(tablero):
#El usuario ingresa coordenadas y se destapa en el tablero la posici�n

  print("Ingrese el numero de la fila y columna que desea destapar")
  print("Ejemplo: 3,4")

  coord = input()
  coordsplited=coord.split(",")
  #print("Separar: ",coordsplited)         #Separar las coord

  if(verificar_dentro(len(tablero),len(tablero[0]),coordsplited)):

    fil = int(coordsplited[0])
    #print("Fila:", fil)
    
    colum = int(coordsplited[1])
    #print("Colum:", colum)
    if(tablero[fil][colum].valor == 9):
      tablero[fil][colum].estaDescubierto = True
      return False
    tablero[fil][colum].estaDescubierto = True

  else:
    print("Ingrese un rango v�lido\n")
  #end if

  return True
#end def


def NumDescubiertas(tablero):

  contDesc = 0;

  for i in range (len(tablero)):
    for j in range (len(tablero[0])):
      if(tablero[i][j].estaDescubierto == True):
        contDesc = contDesc + 1
      #end if
    #end for
  #end for

  #print("Casillas descubiertas: ", contDesc)
  return contDesc
#end def
  
def cicloJuego(tablero, fil, colum, numMina):
  
  estaVivo = True
  tamTablero = fil * colum
  while(NumDescubiertas(tablero) < (tamTablero - numMina) and estaVivo):
    estaVivo = juegaSolito(tablero,fil,colum,numMina)
    imprimirTablero(tablero,"juego")
  #end while
  if(estaVivo):
    print("GANASTEEEE")
  else:
    print("HABEIS PERDIDO TIOOO")
#end def

def juegaSolito(tablero,h,w,n):
  probaEscoger = 1
  pEscoger = [0,0]
  if(NumDescubiertas(tablero)==0):
    for i in tablero:
      for j in i:
        j.proba = n / (h * w)
      #end for
    #end for
    p = [randint(0,h-1),randint(0,w-1)]
    print ("jugada de solito: ",p)
    if(tablero[p[0]][p[1]].valor == 9):
      tablero[p[0]][p[1]].estaDescubierto = True
      return False
    tablero[p[0]][p[1]].estaDescubierto = True
    actualizar_probas(tablero,p)
    return True
  else:
    for i in range(len(tablero)):
      for j in range(len(tablero[0])):
        if tablero[i][j].estaDescubierto:
          p_si = [i-1,j-1]            #punto superior izquierda
          p_sc = [i-1,j]              #punto superior centro
          p_sd = [i-1,j+1]            #punto superior derecho

          p_ci = [i,j-1]              #punto centro izquierda
          #p_cc = [i,j]               #punto centro centro
          p_cd = [i,j+1]              #punto centro derecho

          p_ii = [i+1,j-1]            #punto inferior izquierda
          p_ic = [i+1,j]              #punto inferior centro
          p_id = [i+1,j+1]            #punto inferior derecho

          if(verificar_dentro(h,w,p_si)  and not tablero[p_si[0]][p_si[1]].estaDescubierto and probaEscoger >= tablero[p_si[0]][p_si[1]].proba):
            probaEscoger = tablero[p_si[0]][p_si[1]].proba
            pEscoger[0] = p_si[0]
            pEscoger[1] = p_si[1]
          #endif
          if(verificar_dentro(h,w,p_sc) and not tablero[p_sc[0]][p_sc[1]].estaDescubierto and probaEscoger >= tablero[p_sc[0]][p_sc[1]].proba):
            probaEscoger = tablero[p_sc[0]][p_sc[1]].proba
            pEscoger[0] = p_sc[0]
            pEscoger[1] = p_sc[1]
          #endif
          if(verificar_dentro(h,w,p_sd)and not tablero[p_sd[0]][p_sd[1]].estaDescubierto and probaEscoger >= tablero[p_sd[0]][p_sd[1]].proba):
            probaEscoger = tablero[p_sd[0]][p_sd[1]].proba
            pEscoger[0] = p_sd[0]
            pEscoger[1] = p_sd[1]
          #endif
          if( verificar_dentro(h,w,p_ci) and not tablero[p_ci[0]][p_ci[1]].estaDescubierto and probaEscoger >= tablero[p_ci[0]][p_ci[1]].proba):
            probaEscoger = tablero[p_ci[0]][p_ci[1]].proba
            pEscoger[0] = p_ci[0]
            pEscoger[1] = p_ci[1]
          #endif
          if(verificar_dentro(h,w,p_cd) and not tablero[p_cd[0]][p_cd[1]].estaDescubierto and probaEscoger >= tablero[p_cd[0]][p_cd[1]].proba):
            probaEscoger = tablero[p_cd[0]][p_cd[1]].proba
            pEscoger[0] = p_cd[0]
            pEscoger[1] = p_cd[1]
          #endif
          if(verificar_dentro(h,w,p_ii)  and not tablero[p_ii[0]][p_ii[1]].estaDescubierto and probaEscoger >= tablero[p_ii[0]][p_ii[1]].proba):
            probaEscoger = tablero[p_ii[0]][p_ii[1]].proba
            pEscoger[0] = p_ii[0]
            pEscoger[1] = p_ii[1]
          #endif
          if(verificar_dentro(h,w,p_ic) and not tablero[p_ic[0]][p_ic[1]].estaDescubierto and probaEscoger >= tablero[p_ic[0]][p_ic[1]].proba):
            probaEscoger = tablero[p_ic[0]][p_ic[1]].proba
            pEscoger[0] = p_ic[0]
            pEscoger[1] = p_ic[1]
          #endif
          if(verificar_dentro(h,w,p_id) and not tablero[p_id[0]][p_id[1]].estaDescubierto and probaEscoger >= tablero[p_id[0]][p_id[1]].proba):
            probaEscoger = tablero[p_id[0]][p_id[1]].proba
            pEscoger[0] = p_id[0]
            pEscoger[1] = p_id[1]
          #endif
        #endif
      #endfor
    #endfor
    print ("jugada de solito: ",pEscoger)
    if(tablero[pEscoger[0]][pEscoger[1]].valor == 9):
      tablero[pEscoger[0]][pEscoger[1]].estaDescubierto = True
      return False
    tablero[pEscoger[0]][pEscoger[1]].estaDescubierto = True
    actualizar_probas(tablero,pEscoger)
    return True
  #endif
#end def

def actualizar_probas(tablero,p):
  h = len(tablero)
  w = len(tablero[0])
  probaDes = tablero [p[0]][p[1]].valor
  i = p[0]
  j = p[1]
  p_si = [i-1,j-1]            #punto superior izquierda
  p_sc = [i-1,j]              #punto superior centro
  p_sd = [i-1,j+1]            #punto superior derecho

  p_ci = [i,j-1]              #punto centro izquierda
  #p_cc = [i,j]               #punto centro centro
  p_cd = [i,j+1]              #punto centro derecho

  p_ii = [i+1,j-1]            #punto inferior izquierda
  p_ic = [i+1,j]              #punto inferior centro
  p_id = [i+1,j+1]            #punto inferior derecho

  contPosibles = 0

  if(verificar_dentro(h,w,p_si) and not tablero[p_si[0]][p_si[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_sc) and not tablero[p_sc[0]][p_sc[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_sd) and not tablero[p_sd[0]][p_sd[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_ci) and not tablero[p_ci[0]][p_ci[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_cd) and not tablero[p_cd[0]][p_cd[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_ii) and not tablero[p_ii[0]][p_ii[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_ic) and not tablero[p_ic[0]][p_ic[1]].estaDescubierto):
    contPosibles += 1
  #endif
  if(verificar_dentro(h,w,p_id) and not tablero[p_id[0]][p_id[1]].estaDescubierto):
    contPosibles += 1
  #endif
 ############## para actualizar proba
  if contPosibles == 0:
    contPosibles = 10000
  if(verificar_dentro(h,w,p_si)):
    if(probaDes == 0):
      tablero[p_si[0]][p_si[1]].proba = 0
    elif tablero[p_si[0]][p_si[1]].proba == 0:
      x=0
    else:
      tablero[p_si[0]][p_si[1]].proba = max(tablero[p_si[0]][p_si[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_sc)):
    if(probaDes == 0):
      tablero[p_sc[0]][p_sc[1]].proba = 0
    elif tablero[p_sc[0]][p_sc[1]].proba == 0:
      x=0
    else:
      tablero[p_sc[0]][p_sc[1]].proba = max(tablero[p_sc[0]][p_sc[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_sd)):
    if(probaDes == 0):
      tablero[p_sd[0]][p_sd[1]].proba = 0
    elif tablero[p_sd[0]][p_sd[1]].proba == 0:
      x=0
    else:
      tablero[p_sd[0]][p_sd[1]].proba = max(tablero[p_sd[0]][p_sd[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_ci)):
    if(probaDes == 0):
      tablero[p_ci[0]][p_ci[1]].proba = 0
    elif tablero[p_ci[0]][p_ci[1]].proba == 0:
      x=0
    else:
      tablero[p_ci[0]][p_ci[1]].proba = max(tablero[p_ci[0]][p_ci[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_cd)):
    if(probaDes == 0):
      tablero[p_cd[0]][p_cd[1]].proba = 0
    elif tablero[p_cd[0]][p_cd[1]].proba == 0:
      x=0
    else:
      tablero[p_cd[0]][p_cd[1]].proba = max(tablero[p_cd[0]][p_cd[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_ii)):
    if(probaDes == 0):
      tablero[p_ii[0]][p_ii[1]].proba = 0
    elif tablero[p_ii[0]][p_ii[1]].proba == 0:
      x=0
    else:
      tablero[p_ii[0]][p_ii[1]].proba = max(tablero[p_ii[0]][p_ii[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_ic)):
    if(probaDes == 0):
      tablero[p_ic[0]][p_ic[1]].proba = 0
    elif tablero[p_ic[0]][p_ic[1]].proba == 0:
      x=0
    else:
      tablero[p_ic[0]][p_ic[1]].proba = max(tablero[p_ic[0]][p_ic[1]].proba, probaDes/contPosibles)
  #endif
  if(verificar_dentro(h,w,p_id)):
    if(probaDes == 0):
      tablero[p_id[0]][p_id[1]].proba = 0
    elif tablero[p_id[0]][p_id[1]].proba == 0:
      x=0
    else:
      tablero[p_id[0]][p_id[1]].proba = max(tablero[p_id[0]][p_id[1]].proba, probaDes/contPosibles)
  #endif
#enddef



print("ingrese el numero de filas")
h = int(input())
print("ingrese el numero de columnas")
w = int(input())
print("ingrese el numero de minas")
n = int(input())

print("se tiene un tablero de ",h,"x",w,"con ",n,"minas")
tablero = setupTablero(h,w,n)
print(" ")
print("==========Empieza el juego!===========")
imprimirTablero(tablero,"juego")
print("")
cicloJuego(tablero, h, w, n)