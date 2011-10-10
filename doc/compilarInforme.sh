#!/bin/bash
nombre="Informe"

# Compilo dos veces para que aparezca el índice
pdflatex $nombre.tex
pdflatex $nombre.tex

#Remuevo los archivos temporales que crea la compilación
rm $nombre.out $nombre.toc $nombre.aux $nombre.log
 

