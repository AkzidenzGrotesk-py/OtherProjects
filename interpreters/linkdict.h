#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define int long long

/****** LINKED LISTS ******/
// adapted: https://www.learn-c.org/en/Linked_lists
typedef struct ll_val {
	int llType;
	int ivalue;
	float fvalue;
	char* svalue;
} LLValue;

typedef struct ll_node {
	char *key;
	struct ll_val * val;
	struct ll_node * next;
} LLItem;

// Generate LLValue
LLValue* ciLLV(int data) {
	LLValue *tData = (LLValue *) malloc (sizeof(LLValue));
	tData->llType = 0; tData->ivalue = data;
	return tData;
}

LLValue* cfLLV(float data) {
	LLValue *tData = (LLValue *) malloc (sizeof(LLValue));
	tData->llType = 1; tData->fvalue = data;
	return tData;
}

LLValue* csLLV(char *data) {
	LLValue *tData = (LLValue *) malloc (sizeof(LLValue));
	tData->llType = 2; tData->svalue = data;
	return tData;
}

// Print linked list
void llprintf(LLItem * head) {
  LLItem * current = head;

  printf("\x1b[33m--> DATA\x1b[0m\n");

  while (current != NULL) {
		if (current->val->llType == 0) {
			printf("%s : %d\n", current->key, current->val->ivalue);
		}
		if (current->val->llType == 1) {
			printf("%s : %f\n", current->key, current->val->fvalue);
		}
		if (current->val->llType == 2) {
			printf("%s : %s\n", current->key, current->val->svalue);
		}
    current = current->next;
  }
}

// Get item by key
LLValue* llget(LLItem * head, char* key) {
	int retval = -1;
	LLItem * current = head;
	while (current->next != NULL && strcmp(current->key, key) != 0) {
		current = current->next;
	}

	return current->val;
}

// Add item to end of list
void llappend(LLItem * head, LLValue* val, char* key) {
	LLItem * current = head;
	while (current->next != NULL) {
    // replace if matching key
    if (strcmp(current->next->key, key) == 0) {
      current->next->val = val;
      return;
    }
		current = current->next;
	}

	current->next = (LLItem *) malloc(sizeof(LLItem));
	current->next->val = val;
	current->next->key = key;
	current->next->next = NULL;

  // printf("appended '%s' : '%d'\n", key, val);
}

// Add item to beginning of list
void llpush(LLItem ** head, LLValue* val, char* key) {
	LLItem * new_node;
	new_node = (LLItem *) malloc(sizeof(LLItem));
	new_node->val = val;
	new_node->key = key;
	new_node->next = *head;
	*head = new_node;
}

// Remove first item returns value
LLValue* llpop(LLItem ** head) {
    LLValue* retval = NULL;
    LLItem * next_node = NULL;

    if (*head == NULL) {
        return NULL;
    }

    next_node = (*head)->next;
    retval = (*head)->val;

    free(*head);
    *head = next_node;

    return retval;
}

// Remove last item returns value
LLValue* llremlast(LLItem * head) {
    LLValue* retval = NULL;
    /* if there is only one item in the list, remove it */
    if (head->next == NULL) {
        retval = head->val;
        free(head);
        return retval;
    }

    /* get to the second to last node in the list */
    LLItem * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so let's remove current->next */
    retval = current->next->val;
    free(current->next);
    current->next = NULL;
    return retval;
}

// Remove by index returns value
LLValue* llremindex(LLItem ** head, int n) {
    int i = 0;
    LLValue* retval = NULL;
    LLItem * current = *head;
    LLItem * temp_node = NULL;

    if (n == 0) {
        return llpop(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return NULL;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->val;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}
