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

int count_if_statements(cJSON* json) {
    int ifCount = 0;

    // 현재 JSON 객체가 If 문인지 확인
    cJSON* nodetype = cJSON_GetObjectItem(json, "_nodetype");
    if (nodetype != NULL && cJSON_IsString(nodetype) && strcmp(nodetype->valuestring, "If") == 0) {
        ifCount++;
    }

    // JSON 객체 내의 모든 키를 반복하면서 중첩된 If 문 찾기
    cJSON* child = json->child;
    while (child != NULL) {
        ifCount += count_if_statements(child);  // 재귀적으로 하위 JSON 객체를 확인
        child = child->next;
    }

    return ifCount;
}


int analysis(cJSON *ast) {
    int funcCount = 0;


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
    // ... (이전 코드)

    cJSON_ArrayForEach(element, ext) {
    cJSON *nodetype = cJSON_GetObjectItem(element, "_nodetype");
    if (nodetype != NULL && cJSON_IsString(nodetype) && strcmp(nodetype->valuestring, "FuncDef") == 0) {
        funcCount++;
        
    } else {
        continue;  // 만약 nodetype이 FuncDef가 아니라면, 다음 element로 넘어갑니다.
    }

    // 여기서부터는 nodetype이 "FuncDef"인 경우만 처리합니다.
    printf("Function num:%d\n", funcCount);
    
    cJSON *body = cJSON_GetObjectItem(element, "body");
        if (body != NULL) {
                cJSON *block_items = cJSON_GetObjectItem(body, "block_items");
                if (block_items != NULL) {
                    int if_count = count_if_statements(block_items);
                    printf("If statement count: %d\n", if_count);
                }
            }

    cJSON *decl = cJSON_GetObjectItem(element, "decl");  // nodetype이 아닌 element에서 "decl"을 가져옵니다.
    if (decl == NULL) {
        //fprintf(stderr, "decl not found\n");
        continue;  // decl이 없다면, 다음 element로 넘어갑니다.
    }

    cJSON *funcName = cJSON_GetObjectItem(decl, "name");
    if (funcName != NULL && cJSON_IsString(funcName)) {  // name이 존재하고 문자열인지 확인합니다.
        printf("funcName: %s\n", funcName->valuestring);
    } else {
        printf("funcName: NULL\n");
    }

    // ... (이후 코드도 비슷한 방식으로 수정)

    // 반환 타입을 가져오는 코드 부분
    cJSON *type1 = cJSON_GetObjectItem(decl, "type");
    if (type1 == NULL) {
        //fprintf(stderr, "type1 not found\n");
        continue;
    }

    cJSON *TypeDecl = cJSON_GetObjectItem(type1, "type");
    if (TypeDecl == NULL) {
        //fprintf(stderr, "TypeDecl not found\n");
        continue;
    }

    cJSON *IdentifierType = cJSON_GetObjectItem(TypeDecl, "type");
    if (IdentifierType == NULL) {
        //fprintf(stderr, "IdentifierType not found\n");
        continue;
    }

    cJSON *returnType = cJSON_GetObjectItem(IdentifierType, "names");
    if (returnType != NULL && cJSON_IsArray(returnType)) {  // names가 배열인지 확인합니다.
        cJSON *firstItem = cJSON_GetArrayItem(returnType, 0);
        if (firstItem != NULL && cJSON_IsString(firstItem)) {  // 배열의 첫 번째 항목이 문자열인지 확인합니다.
            printf("returnType: %s\n", firstItem->valuestring);
        }
    } else {
        printf("returnType: NULL\n");
    }

      // 매개변수를 가져오는 코드 부분
    cJSON *args = cJSON_GetObjectItem(type1, "args");
    if (args == NULL) {
        //fprintf(stderr, "args not found\n");
        continue;
    }

    cJSON *params = cJSON_GetObjectItem(args, "params");
    if (params == NULL) {
        fprintf(stderr, "params not found\n");
        printf("\n\n");
        continue;
    }

    // 매개변수 이름을 가져오는 코드 부분
    cJSON *paramsName = cJSON_GetObjectItem(params, "name");
    if (paramsName != NULL && cJSON_IsString(paramsName)) {
        printf("paramsName: %s\n", paramsName->valuestring);
    } else {
        printf("paramsName: NULL\n");
    }

    // 매개변수 타입을 가져오는 코드 부분
    cJSON *type2 = cJSON_GetObjectItem(params, "type");
    if (type2 == NULL) {
        //fprintf(stderr, "type2 not found\n");
        continue;
    }

    cJSON *type3 = cJSON_GetObjectItem(type2, "type");
    if (type3 == NULL) {
        //fprintf(stderr, "type3 not found\n");
        continue;
    }

    cJSON *paramsType = cJSON_GetObjectItem(type3, "names");
    if (paramsType != NULL && cJSON_IsArray(paramsType)) {
        cJSON *firstItem = cJSON_GetArrayItem(paramsType, 0);
        if (firstItem != NULL && cJSON_IsString(firstItem)) {
            printf("paramsType: %s\n", firstItem->valuestring);
        }
    } else {
        printf("paramsType: NULL\n");
    }


        printf("\n\n");
    }

    printf("\nFunction Total: %d\n", funcCount);
    return 0;
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

    analysis(ast);
    

    // 메모리 해제
    cJSON_Delete(ast);
    free(json_string);

    return 0;
}

