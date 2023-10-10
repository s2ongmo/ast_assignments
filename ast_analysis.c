#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

/*
<기본> 
함수 개수 추출 함수
함수들의 리턴타입 추출 함수
함수들의 이름 추출 함수
함수들의 파라미터 타입, 변수명 추출 함수
함수들의 if 조건 개수 추출하기
*/

int supports_full_hd(const char * const monitor_json_string)
{
    // JSON 파싱
    cJSON *monitor_json = cJSON_Parse(monitor_json_string);
    if (monitor_json == NULL)
    {
        fprintf(stderr, "Error: Could not parse JSON\n");
        return 0;
    }

    // name 필드 출력 (옵션)
    cJSON *name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    // 해상도 검사
    cJSON *resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON *resolution = NULL;
    cJSON_ArrayForEach(resolution, resolutions)
    {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
        {
            fprintf(stderr, "Error: Resolution width/height is not a number\n");
            cJSON_Delete(monitor_json);
            return 0;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
        {
            cJSON_Delete(monitor_json);
            return 1;
        }
    }

    // 메모리 해제 및 종료
    cJSON_Delete(monitor_json);
    return 0;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // 파일 읽기
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *data = malloc(length + 1);
    if (!data) {
        perror("malloc");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);
    
    // 함수 호출 및 결과 출력
    int result = supports_full_hd(data);
    if (result) {
        printf("The monitor supports Full HD.\n");
    } else {
        printf("The monitor does not support Full HD.\n");
    }
    
    free(data);
    return EXIT_SUCCESS;
}

