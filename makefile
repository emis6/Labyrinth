# Modèle de fichier Makefile  pour le fichier template.c
LIBDIR = /home/mahshid/Documents/C/Project/C_Projet/CGS_lib/

# options de compilation
CC = gcc
CCFLAGS = -Wall -I $(LIBDIR)/include
LIBS = -L $(LIBDIR)/lib
LDFLAGS = -lm -lcgs

# fichiers du projet
SRC = my_heap.h my_heap.c template.c
OBJ = $(SRC:.c=.o)
EXEC = template


# règle initiale
all: $(EXEC)

# dépendance des .h
template.o:

# règles de compilation
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<
	
# règles d'édition de liens
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

# règles supplémentaires
clean:
	rm -f *.o
rmproper:
	rm -f $(EXEC) *.o