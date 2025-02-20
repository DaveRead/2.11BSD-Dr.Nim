/**
 * Dr. Nim Game
 * 
 * Written to compile on the PiDP 11 running 2.11 BSD.
 * Also written to be run from the httpd PiDP server
 * with request payload support.
 * 
 * A new game is assumed if remaining_marbles=15 and player_takes=0.
 * 
 * Dave Read
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEFAULT_MARBLE_COUNT 15
#define WEB_APP_PATH "/cgi-bin/drnim_webcgi"
#define MARBLE_IMAGE_PATH "/images/marble.png"
#define REMAINING_MARBLES_HTTP_REQ_PARAM "remaining_marbles"
#define PLAYER_TAKES_HTTP_REQ_PARAM "player_takes"

#define PLAYER_STATUS_OK 0
#define PLAYER_STATUS_ERROR -1

int marbles;


/* Function prototypes */
void run_game();
void show_queued_marbles();
int player_turn();
void dr_nim_turn();
int get_value_for_req_param();
void html_header();
void html_footer();
void html_new_game();
void html_play_again();
void html_player_input();
void show_marbles_taken();

/* Start the program */
int main(argc, argv)
int argc;
char **argv;
{
    int player_takes = get_value_for_req_param(argv, PLAYER_TAKES_HTTP_REQ_PARAM, 0, 3, 0);
    marbles = get_value_for_req_param(argv, REMAINING_MARBLES_HTTP_REQ_PARAM, 0, 15, 15);
    run_game(player_takes);
    return 0;
}

/* Control game logic */
void run_game(player_takes)
int player_takes;
{
    int player_status;

    html_header();

    printf("<!-- Marbles remaining param:%d  Player takes param:%d -->\n", 
        marbles, player_takes);

    if (marbles == 15 && player_takes == 0) {
        html_new_game();
    } else {
        if (marbles) {
            player_status = player_turn(player_takes);
        }

        if (marbles && player_status == PLAYER_STATUS_OK) {
            dr_nim_turn();
        }

        if (marbles) {
            printf("<hr><br/>\n");
            show_queued_marbles();
            html_player_input();
        } else {
            html_play_again();
        }
    }

    html_footer();
}

/* Display the remaining marbles */
void show_queued_marbles()
{
    int m;

    if (!marbles) {
        return;
    }

    printf("<table style=\"width: 100%%; border: thick outset; padding 3px\">\n");
    printf("<tr><td style=\"border: thin inset; margin: 3px\">");
    for (m = 0;m < marbles;m++) {
        if (MARBLE_IMAGE_PATH == NULL || strlen(MARBLE_IMAGE_PATH) == 0) {
            printf("O&nbsp;");
        } else {
            printf("<img src='%s'alt='marble image'/>&nbsp;", MARBLE_IMAGE_PATH);
        }
    }
    printf("</td></tr></table>\n");
    printf("<br/>\n");
}

/* Process the player's input */
int player_turn(player_takes)
int player_takes;
{
    if (player_takes == 0) {
        printf("<h3><font color=\"orange\">Oops!</font> ");
        printf("You didn't take any marbles. You must take at least one.</h3>\n");
        return PLAYER_STATUS_ERROR;
    } else if (player_takes > marbles) {
        printf("<h3><font color=\"orange\">Oops!</font> ");
        printf("You tried to take %d marbles, but only %d remain%s</h3>\n", 
            player_takes, marbles, (marbles == 1? "s" : ""));
        return PLAYER_STATUS_ERROR;
    }
    
    printf("<table cellspacing='10' cellpadding='0' border='0'><tr><td valign='center'>");
    printf("<font size='+3'><strong>You took %d marble%s", 
        player_takes, player_takes != 1? "s" : "");

    marbles -= player_takes;

    if (!marbles) {
        if (player_takes > 1) {
            printf(", which were all the remaining marbles.</strong></font>\n");
        } else {
            printf(", which was the last marble.</strong></font>\n");
        }
    } else {
        printf("</strong></font>\n");
    }

    printf("</td><td valign='center'>");
    show_marbles_taken(player_takes);
    printf("</td></tr></table>\n");
    

    /* Player took the last marble, Dr. Nim wins*/
    if (!marbles) {
        printf("<table cellspacing='10' cellpadding='0' border='0'><tr><td>");
        printf("<font size='+4' color='#079090'>Dr. Nim wins!</font>\n");
        printf("</td></tr></table>\n<hr>\n");
    }

    printf("</table>");
    
    return PLAYER_STATUS_OK;
}

/* Run Dr. Nim's logic */
void dr_nim_turn()
{
    /* See if Dr. Nim take the optimal number of marbles to win */
    int take = (marbles - 1) % 4;

    /* The player currently has an advantage, 
        take a random number of marbles */
    if (!take) {
        take = rand() % 3 + 1;
        if (take > marbles) {
            take = marbles - 1;
        }
        if (!take) {
            take = 1;
        }
    }
    
    printf("<table cellspacing='10' cellpadding='0' border='0'><tr><td valign='center'>");

    if (marbles == 1) {
        printf("<font size='+3'><strong>Dr. Nim took the last marble</strong></font>");
    } else {
        printf("<font size='+3'><strong>Dr. Nim took %d marble%s</strong></font>\n", 
            take, take != 1? "s" : "");
    }

    printf("</td><td valign='center'>");
    show_marbles_taken(take);
    printf("</td></tr></table>");

    marbles -= take;

    /* Dr. Nim took the last marble, player wins*/
    if (!marbles) {
        printf("<table cellspacing='10' cellpadding='0' border='0'><tr><td>");
        printf("<font size='+4' color='#7070f0'>Congratulations!</font>");
        printf("<font size='+4'> You beat Dr. Nim!</font>");
        printf("</td></tr></table>\n<hr>\n");
    }
}

/* Locate the request parameter and return its associated value.
    For simplicity, each parameter's value is a single digit value (in hex). 
    If the value found is outside the [min,max] range, the default value is returned. */
int get_value_for_req_param(argv, request_param, min, max, default_value)
char **argv;
char *request_param;
int min, max, default_value;
{
    int value = default_value;
    char *param;

    while (*argv) {
        if ((param = strstr(*argv, request_param)) != NULL && 
                *(param + strlen(request_param)) == '=') {
            value = *(param + strlen(request_param) + 1);
            if (value >= '0' && value <= '9') {
                value -= '0';
            } else if (toupper(value) >= 'A' && toupper(value) <= 'F') {
                value = 10 + (toupper(value) - 'A');
            }
            break;
        }
        argv++;
    }
    
    if (value < min || value > max) {
        value = default_value;
    }

    return value;
}
    
/* The header sent to begin each response */
void html_header() {
    printf("HTTP/1.1 200 OK\n");
    printf("Content-Type: text/html; charset=utf-8\n");
    printf("Cache-Control: no-store, no-cache\n");
    printf("max-age=0\n");
    printf("Expires: -1\n");
    printf("\n");
    printf("<html>\n");
    printf("<head>");
    printf("  <title>Dr. Nim</title>\n");
    printf("<style>\n");
    printf("table.tb1 { border: 5px bevel; elevation: 2 }\n");
    printf("table.tb1 tr { border: 4px bevel; elevation: -2 }\n");
    printf("</style>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h1>Dr. Nim</h1><hr/>");
}

/* The footer sent at the end of each response */
void html_footer() {
    printf("<hr>\n");
    printf("<p><font size='-2'>Dr. Nim for the PiDP-11</font><p>\n");
    printf("</body>");
    printf("</html>\n");
}

/* HTML to present the user with an option to play again */
void html_play_again() {
    printf("<table cellpadding=\"2\" bgcolor=\"#70f070\" border=\"0\"><tr><td align=\"center\">");
    printf("<h2>Game Over</h2>\n");
    printf("<form action='%s' method='post'>\n", WEB_APP_PATH);
    printf("<input type='submit' value='Play Again'>");
    printf("</form>\n");
    printf("</td></tr></table>");
}

/* HTML for the form asking for the user's input */
void html_player_input() {
    int i;
    static char *numbers[] = { "One", "Two", "Three" };
    /* printf("<hr/>\n"); */
    printf("<table bgcolor='#f0f0ff' border='0'>");
    printf("<form action='%s', method='POST'>\n", WEB_APP_PATH);
    printf("<input type='hidden' name='%s' value='%x'>\n", REMAINING_MARBLES_HTTP_REQ_PARAM, marbles);
    printf("<tr><th>How many marbles are you taking?</th></tr>\n");
    for (i = 0;i < 3 && i < marbles;i++) {
        printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;<input type='radio' name='%s' value='%d' %s> %s</td></tr>\n", 
            PLAYER_TAKES_HTTP_REQ_PARAM, (i + 1), 
            (i == 0? "checked" : ""), numbers[i]);
    }
    printf("<tr><td align='center'><input type='submit' value='Take Marble%s'></td></tr>\n", (marbles > 1? "(s)" : ""));
    printf("</form>\n");
    printf("</table>\n");
}

/* HTML to introduce a new game */
void html_new_game() {
    printf("<h2>New Game</h2>");
    printf("<p>You may take between 1 and 3 marbles during your turn.</p>");
    printf("<h3>Don't get stuck with the last marble!</h3>");
    printf("<hr>\n");
    show_queued_marbles();
    html_player_input();   
}

/* HTML to display the number of marbles taken */
void show_marbles_taken(num_taken) 
int num_taken;
{
    int m;

    for (m = 0;m < num_taken;m++) {
        if (MARBLE_IMAGE_PATH == NULL || strlen(MARBLE_IMAGE_PATH) == 0) {
            printf("O&nbsp;");
        } else {
            printf("<img src='%s' alt='marble image' width='26' height='26'/>&nbsp;", MARBLE_IMAGE_PATH);
        }
    }
}