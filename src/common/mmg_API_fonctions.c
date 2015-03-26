/* =============================================================================
**  This file is part of the mmg software package for the tetrahedral
**  mesh modification.
**  Copyright (c) Inria - IMB (Université de Bordeaux) - LJLL (UPMC), 2004- .
**
**  mmg is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published
**  by the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  mmg is distributed in the hope that it will be useful, but WITHOUT
**  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
**  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
**  License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License and of the GNU General Public License along with mmg (in
**  files COPYING.LESSER and COPYING). If not, see
**  <http://www.gnu.org/licenses/>. Please read their terms carefully and
**  use this copy of the mmg distribution only if you accept them.
** =============================================================================
*/

/**
 * \file common/mmg_API_functions.c
 * \brief C API functions definitions for MMG library.
 * \author Algiane Froehly (Inria / IMB, Université de Bordeaux)
 * \version 5
 * \date 01 2014
 * \copyright GNU Lesser General Public License.
 *
 * \note This file contains some internal functions for the API, see the \ref
 * common/libmmg.h, \ref mmgs/libmmgs.h and \ref mmg3d/libmmg3d.h header files
 * for the documentation of all the usefull user's API functions.
 *
 * C API for MMG library.
 *
 */

#include "mmg.h"

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 *
 * Allocate the mesh and solutions structures at \a MMG3D format.
 *
 */
static inline
void MMG5_Alloc_mesh(MMG5_pMesh *mesh, MMG5_pSol *sol
  ) {

  /* mesh allocation */
  if ( *mesh )  _MMG5_SAFE_FREE(*mesh);
  _MMG5_SAFE_CALLOC(*mesh,1,MMG5_Mesh);

  /* sol allocation */
  if ( *sol )  _MMG5_DEL_MEM(*mesh,*sol,sizeof(MMG5_Sol));
  _MMG5_SAFE_CALLOC(*sol,1,MMG5_Sol);

  return;
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 *
 * Initialization of mesh and solution structures to their default
 * values (default names, versions, dimensions...).
 *
 */
static inline
void MMG5_Init_woalloc_mesh(MMG5_pMesh mesh, MMG5_pSol sol
  ) {

  (mesh)->dim = 3;
  (mesh)->ver = 2;
  (sol)->dim  = 3;
  (sol)->ver  = 2;
  (sol)->size = 1;

  /* Default parameters values */
  MMG5_Init_parameters(mesh);

  /* Default vaules for file names */
  MMG5_Init_fileNames(mesh,sol);

  return;
}

/**
 * \param mesh pointer toward a pointer toward the mesh structure.
 * \param sol pointer toward a pointer toward the sol structure.
 *
 * Allocate the mesh and solution structures and initialize it to
 * their default values.
 *
 */
void MMG5_Init_mesh(MMG5_pMesh *mesh, MMG5_pSol *sol
  ) {

  /* allocations */
  MMG5_Alloc_mesh(mesh,sol);
  /* initialisations */
  MMG5_Init_woalloc_mesh(*mesh,*sol);

  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 *
 * Initialize file names to their default values.
 *
 */
void MMG5_Init_fileNames(MMG5_pMesh mesh,MMG5_pSol sol
  ) {
  MMG5_Set_inputMeshName(mesh,"");
  MMG5_Set_outputMeshName(mesh,"");

  MMG5_Set_inputSolName(mesh,sol,"");
  MMG5_Set_outputSolName(mesh,sol,"");

  return;
}


/**
 * \param mesh pointer toward the mesh structure.
 * \param meshin input mesh name.
 * \return 1.
 *
 * Set the name of input mesh.
 *
 */
int MMG5_Set_inputMeshName(MMG5_pMesh mesh, char* meshin) {

  if ( mesh->namein ){
    _MMG5_DEL_MEM(mesh,mesh->namein,(strlen(mesh->namein)+1)*sizeof(char));
  }

  if ( strlen(meshin) ) {
    _MMG5_ADD_MEM(mesh,(strlen(meshin)+1)*sizeof(char),"input mesh name",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(mesh->namein,strlen(meshin)+1,char);
    strcpy(mesh->namein,meshin);
  }
  else {
    _MMG5_ADD_MEM(mesh,10*sizeof(char),"input mesh name",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(mesh->namein,10,char);
    strcpy(mesh->namein,"mesh.mesh");
    if ( (mesh->info.imprim > 5) || mesh->info.ddebug ) {
      fprintf(stdout,"  ## Warning: no name given for input mesh.\n");
      fprintf(stdout,"     Use of default value \"mesh.mesh\".\n");
    }
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param solin name of the input solution file.
 * \return 1.
 *
 * Set the name of input solution file.
 *
 */
int MMG5_Set_inputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solin) {
  char *ptr;

  if ( sol->namein )
    _MMG5_DEL_MEM(mesh,sol->namein,(strlen(sol->namein)+1)*sizeof(char));
  if ( strlen(solin) ) {
    _MMG5_ADD_MEM(mesh,(strlen(solin)+1)*sizeof(char),"input sol name",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(sol->namein,strlen(solin)+1,char);
    strcpy(sol->namein,solin);
  }
  else {
    if ( strlen(mesh->namein) ) {
      _MMG5_SAFE_CALLOC(sol->namein,strlen(mesh->namein)+1,char);
      strcpy(sol->namein,mesh->namein);
      ptr = strstr(sol->namein,".mesh");
      if ( ptr ) {
        /* the sol file is renamed with the meshfile without extension */
        *ptr = '\0';
        _MMG5_SAFE_REALLOC(sol->namein,(strlen(sol->namein)+1),char,"input sol name");
      }
      _MMG5_ADD_MEM(mesh,(strlen(sol->namein)+1)*sizeof(char),"input sol name",
                    printf("  Exit program.\n");
                    exit(EXIT_FAILURE));
    }
    else {
      _MMG5_ADD_MEM(mesh,9*sizeof(char),"input sol name",
                    printf("  Exit program.\n");
                    exit(EXIT_FAILURE));
      _MMG5_SAFE_CALLOC(sol->namein,9,char);
      strcpy(sol->namein,"mesh.sol");
    }
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param solout name of the output solution file.
 * \return 0 if failed, 1 otherwise.
 *
 *  Set the name of output solution file.
 *
 */
int MMG5_Set_outputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solout) {
  char *ptr;

  if ( sol->nameout )
    _MMG5_DEL_MEM(mesh,sol->nameout,(strlen(sol->nameout)+1)*sizeof(char));

  if ( strlen(solout) ) {
    _MMG5_ADD_MEM(mesh,(strlen(solout)+1)*sizeof(char),"output sol name",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(sol->nameout,strlen(solout)+1,char);
    strcpy(sol->nameout,solout);
  }
  else {
    if ( strlen(mesh->nameout) ) {
      ptr = strstr(mesh->nameout,".mesh");
      if ( ptr )
        _MMG5_SAFE_CALLOC(sol->nameout,strlen(mesh->nameout)+1,char);
      else
        _MMG5_SAFE_CALLOC(sol->nameout,strlen(mesh->nameout)+5,char);
      strcpy(sol->nameout,mesh->nameout);
      ptr = strstr(sol->nameout,".mesh");
      if ( ptr )
        /* the sol file is renamed with the meshfile without extension */
        *ptr = '\0';
      strcat(sol->nameout,".sol");
      _MMG5_ADD_MEM(mesh,(strlen(sol->nameout)+1)*sizeof(char),"output sol name",
                    printf("  Exit program.\n");
                    exit(EXIT_FAILURE));
      _MMG5_SAFE_REALLOC(sol->nameout,(strlen(sol->nameout)+1),char,"output sol name");
    }
    else {
      fprintf(stdout,"  ## Error: no name for output mesh. please, use");
      fprintf(stdout," the MMG5_Set_outputMeshName to set the mesh name.\n");
      return(0);
    }
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward the sol structure.
 *
 * File name deallocations before return.
 *
 */
void MMG5_Free_names(MMG5_pMesh mesh,MMG5_pSol met
  ){
  /* mesh */
  if ( mesh->nameout ) {
    _MMG5_DEL_MEM(mesh,mesh->nameout,(strlen(mesh->nameout)+1)*sizeof(char));
  }

  if ( mesh->namein ) {
    _MMG5_DEL_MEM(mesh,mesh->namein,(strlen(mesh->namein)+1)*sizeof(char));
  }

  /* met */
  if ( met ) {
    if ( met->namein ) {
      _MMG5_DEL_MEM(mesh,met->namein,(strlen(met->namein)+1)*sizeof(char));
    }

    if ( met->nameout ) {
      _MMG5_DEL_MEM(mesh,met->nameout,(strlen(met->nameout)+1)*sizeof(char));
    }
  }
}