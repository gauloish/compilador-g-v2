import subprocess

tests = {
    "correct": [
        "fatorialCorreto.g",
        "selectionSort.g",
        "selectionSortV2.g",
        "selectionSortV3.g",
        "somatorioVetor.g",
        "somatorioVetorV2.g",
        "somaVetores.g",
        "somaVetoresV2.g",
    ],
    "lexical_error": [
        "char_invalido.g",
        "comentario_sem_fim.g",
        "NotaEmConceito-CadeiaCaracteresOcupaMaisDeUmalinhaLin10-11.g",
        "NotaEmConceito-CaractereInvalidoLinha16.g",
        "NotaEmConceito-ComentarioNaoTerminaLin2.g",
        "NotaEmConceito-MaisDeUmCaractereNaConstCarLin55.g",
        "string_newline.g",
    ],
    "syntatic_error": [
        "gv2_funcao_sem_fechamento.g",
        "gv2_varsection_chaves.g",
        "NotaEmConceito-AbreChavesEsperadoLin18.g",
        "NotaEmConceito-DeclaracaoNaoEnvolvidaEmColchetesLin15-17.g",
        "NotaEmConceito-FechaChavesEsperadoLin49.g",
        "NotaEmConceito-FimseFaltandoLIn60.g",
        "NotaEmConceitoPonto-e-virgulaFaltandoLin20.g",
    ],
    "semantic_error": [
        "gv2_arg_vetor_por_escalar.g",
        "gv2_args_errados.g",
        "gv2_atrib_vetor_escalar.g",
        "gv2_funcao_nao_declarada.g",
        "gv2_indice_nao_int.g",
        "gv2_local_nome_param.g",
        "gv2_retorne_tipo_errado.g",
        "gv2_tipo_arg_errado.g",
    ]
}

for i in ["correct", "lexical_error", "syntatic_error", "semantic_error"]: # ["correct", "lexical_error", "syntatic_error", "semantic_error"]:
    print(f"\\\\ ======================================== {i} ======================================== \\\\ \n")

    for test in tests[i]:
        print(f"Arquivo \"{i}/{test}\":")
        subprocess.run(["../g-v2", f"{i}/{test}"])
        print()
