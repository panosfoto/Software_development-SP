#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "error.h"

#define OUTPUT_FILE_NAME "results.txt"
#define BUFFER_SIZE 50

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments provided.\nExpected: ./<executable> <initial graph file> <workload file>\nExiting...\n");
        return -1;
    }
    int node1, node2, ret_val;
    unsigned long long i;
    FILE *initial_graph, *workload, *results;
    char command;
    pGraph graph = gCreateGraph();
    if (graph == NULL)
    {
        print_error();
        return -1;
    }
    if ((initial_graph = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Error opening initial graph file '%s'\nExiting...\n", argv[1]);
        gDestroyGraph(&graph);
        return -1;
    }
    if ((workload = fopen(argv[2], "r")) == NULL)
    {
        fprintf(stderr, "Error opening workload file '%s'\nExiting...\n", argv[2]);
        fclose(initial_graph);
        gDestroyGraph(&graph);
        return -1;
    }
    if ((results = fopen(OUTPUT_FILE_NAME, "w")) == NULL)
    {
        fprintf(stderr, "Error opening (or creating) results file '%s'\nExiting...\n", OUTPUT_FILE_NAME);
        fclose(initial_graph);
        fclose(workload);
        gDestroyGraph(&graph);
        return -1;
    }
    // read initial graph
    for (i = 1 ;; ++i)
    {
        command = fgetc(initial_graph);
        //printf("check: '%c'\n", command); // DEBUG
        if (/*(command = fgetc(initial_graph))*/command == 'S')
            break;
        ungetc(command, initial_graph);
        if (fscanf(initial_graph, "%d %d\n", &node1, &node2) != 2)
        {
            fprintf(stderr, "Error reading initial graph file: did not read two integers from line %llu\nExiting...\n", i);
            fclose(initial_graph);
            fclose(workload);
            fclose(results);
            gDestroyGraph(&graph);
            return -1;
        }
        //printf("%d %d\n", node1, node2); // DEBUG
        ret_val = gAddEdge(graph, node1, node2);
        if (ret_val < 0)
        {
            print_error();
            fprintf(stderr, "Error(s) found while processing initial graph file (line %llu)\nExiting...\n", i);
            fclose(initial_graph);
            fclose(workload);
            fclose(results);
            gDestroyGraph(&graph);
            return -1;
        }
    }
    // process workload
    for (i = 1 ; !feof(workload) ; ++i)
    {
        command = fgetc(workload);
        printf("check: '%c'\n", command); // DEBUG
        if (/*(command = fgetc(workload))*/command == 'S')
            break;
        if (/*(command = fgetc(workload))*/command == 'F')
        {
            //take the '\n' and continue
            fgetc(workload);
            continue;
        }
        ungetc(command, workload);
        if (fscanf(workload, "%c %d %d\n", &command, &node1, &node2) != 3)
        {
            fprintf(stderr, "Error reading workload file: did not read a character and two integers from line %llu\nExiting...\n", i);
            fclose(initial_graph);
            fclose(workload);
            fclose(results);
            gDestroyGraph(&graph);
            return -1;
        }
        printf("%c %d %d\n", command, node1, node2);
        if (command == 'A')
        {
            ret_val = gAddEdge(graph, node1, node2);
            if (ret_val)
            {
                print_error();
                fprintf(stderr, "Error(s) found while processing workload file (line %llu)\nExiting...\n", i);
                fclose(initial_graph);
                fclose(workload);
                fclose(results);
                gDestroyGraph(&graph);
                return -1;
            }
        }
        else if (command == 'Q')
        {
            ret_val = gFindShortestPath(graph, node1, node2, BFS);
            if (ret_val >= 0)
            {
                printf("%d\n", ret_val);
            }
            else if (ret_val == GRAPH_SEARCH_PATH_NOT_FOUND)
            {
                puts("-1");
            }
            else // ret_val < 0, an error occurred (not GRAPH_SEARCH_PATH_NOT_FOUND)
            {
                print_error();
                fprintf(stderr, "Error(s) found while processing workload file (line %llu)\nExiting...\n", i);
                fclose(initial_graph);
                fclose(workload);
                fclose(results);
                gDestroyGraph(&graph);
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "Error reading workload file: command '%c' not recognized (line %llu)\nExiting...\n", command, i);
            fclose(initial_graph);
            fclose(workload);
            fclose(results);
            gDestroyGraph(&graph);
            return -1;
        }
    }

    gDestroyGraph(&graph);
    fclose(initial_graph);
    fclose(workload);
    fclose(results);
    return 0;
}
