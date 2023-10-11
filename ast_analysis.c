#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
<기본> 
함수 개수 추출 함수
함수들의 리턴타입 추출 함수
함수들의 이름 추출 함수
함수들의 파라미터 타입, 변수명 추출 함수
함수들의 if 조건 개수 추출하기
*/


int count_func_defs(cJSON *ast) {
    int count = 0;

    // "ext" 키를 가진 아이템을 가져옵니다.
    cJSON *ext = cJSON_GetObjectItem(ast, "ext");
    if (ext == NULL) {
        fprintf(stderr, "ext not found\n");
        return 0; // ext가 없으면 0을 반환합니다.
    }

    // "ext" 아이템이 배열인지 확인합니다.
    if (!cJSON_IsArray(ext)) {
        fprintf(stderr, "ext is not an array\n");
        return 0; // ext가 배열이 아니면 0을 반환합니다.
    }

    // 배열의 각 요소를 순회합니다.
    cJSON *element;
    cJSON_ArrayForEach(element, ext) {
        // "_nodetype" 키를 가진 아이템을 가져옵니다.
        cJSON *nodetype = cJSON_GetObjectItem(element, "_nodetype");
        if (nodetype != NULL && cJSON_IsString(nodetype) && strcmp(nodetype->valuestring, "FuncDef") == 0) {
            count++; // "_nodetype"이 "FuncDef"이면 카운트를 증가시킵니다.
        }
    }

    return count;
}

void analyze_function(cJSON *function) {
    // 함수 이름 추출
    cJSON *name = cJSON_GetObjectItem(function, "name");
    printf("Function Name: %s\n", name->valuestring);

    // 리턴 타입 추출
    cJSON *returnType = cJSON_GetObjectItem(function, "returnType");
    printf("Return Type: %s\n", returnType->valuestring);

    // 파라미터 추출
    cJSON *parameters = cJSON_GetObjectItem(function, "parameters");
    int param_count = cJSON_GetArraySize(parameters);
    printf("Parameter Count: %d\n", param_count);
    for(int i = 0; i < param_count; i++) {
        cJSON *param = cJSON_GetArrayItem(parameters, i);
        cJSON *param_type = cJSON_GetObjectItem(param, "type");
        cJSON *param_name = cJSON_GetObjectItem(param, "name");
        printf("Parameter: %s %s\n", param_type->valuestring, param_name->valuestring);
    }

    // if문 개수 추출은 AST의 구조에 따라 다르므로, 예시를 제공하기 어렵습니다.
    // 일반적으로 조건문 노드를 찾아서 카운트를 증가시키는 방식으로 구현할 수 있습니다.
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rt");
    if(fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    // 파일 크기 얻기
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 메모리 할당 및 파일 읽기
    char *json_string = (char*)malloc((length + 1) * sizeof(char));
    if(json_string == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(fp);
        return 1;
    }
    fread(json_string, 1, length, fp);
    json_string[length] = '\0';

    // 파일 닫기
    fclose(fp);

    // JSON 파싱
    cJSON *ast = cJSON_Parse(json_string);
    if(ast == NULL) {
        fprintf(stderr, "Error parsing JSON\n");
        free(json_string);
        return 1;
    }

    int func_def_count = count_func_defs(ast);
    printf("FuncDef Count: %d\n", func_def_count);

    // 메모리 해제
    cJSON_Delete(ast);
    free(json_string);

    return 0;
}

