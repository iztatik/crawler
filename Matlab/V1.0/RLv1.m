close all; clear all; clc;
pause on

%%%%%%%%%%%%% 1. Definición de hiperparámetros %%%%%%%%%%%%%%%%%%%%%%% 
% Definición de hiperparámetros de aprenddizaje      
gamma = 0.75;
alpha = 0.1;
epsilon = 0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Variables de los servomotores
global theta_1
theta_1 = 0;
theta1_min = 0;
theta1_max = 60;

global theta_2
theta_2 = 0;
theta2_min = 0;
theta2_max = 60;


% Estados
N_states_s1 = 3;
N_states_s2 = 3;
states_S1 = 1:N_states_s1;
states_S2 = 1:N_states_s2;
N_states = N_states_s1 * N_states_s2;
global actual_state
actual_state = [1,1];
global next_state
next_state = [1,1];

% Acciones
N_actions = 4;
actions = 1:N_actions;
action = 0;

%%%%%%%%%%%%%%%%%% 2. Inicialización de los Q-valores %%%%%%%%%%%%%%%%%%%%%
% Q-Valores ()
q_values = zeros(N_states_s1, N_states_s2, N_actions);

% Evitamos acciones que desborden el espacio de estados
q_values(N_states_s1,:,1) = NaN;
q_values(1,:,2) = NaN;
q_values(:,N_states_s2,3) = NaN;
q_values(:,1,4) = NaN;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Variables del encoder
encoder = 0;
roll_delay = 0.2;  % 200ms

%Variables de tiempo
explorationMin = 5.0;
explorationConst = (explorationMin*60.0)/roll_delay;

% Otras variables
global B;
flag = 0;

% Bucle principal
BT_init();
t = 0;
%%%%%%%%%%%%%%%%%%%%%% 3. Establecemos el estado inicial %%%%%%%%%%%%%%%%%
home(theta1_min, theta2_min);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
fwrite(B,'e'); %Ponemos mensaje de entrenamiento LCD
while true
    % incrementamos el tiempo t
    t = t+1;
    % Calculamos epsilon 
    epsilon = exp(-t/explorationConst);
    %%%% 4. Obtenemos la acción a ejecutar para el estado actual %%%%
    [~, action] = get_maxQ(actual_state, q_values);
    action =  soft_greedy_action(epsilon, action, N_actions, q_values, actual_state);
    
    %%%% 5. Ejecutamos la acción y obtenemos recompensa %%%%
    set_servos(action, theta1_max, theta1_min, theta2_max, theta2_min, N_states_s1, N_states_s2);
    set_action(action, N_states_s1, N_states_s2);
    r = read_encoder(roll_delay);
    
    %%%% 6. Aplicamos la ec. de Bellman %%%%
    if epsilon > 0.1
        [new_maxQ ,action2] = get_maxQ(next_state, q_values);
        sample = r + gamma*(new_maxQ);
        q_values(actual_state(1),actual_state(2),action) = q_values(actual_state(1),actual_state(2),action) + alpha*(sample - q_values(actual_state(1),actual_state(2),action));
    elseif flag == 0
        fwrite(B,'f');
        pause(0.02);
        flag =1;
    end
    
    %%%% 7. Actualizamos el estado actual %%%%
    actual_state = next_state;
end


% Funciones

function BT_init()                                 %Inicializa la conexión Bluetooth
global B
disp("Iniciando conexión...") 
B = Bluetooth('Crawler',1);
fopen(B);
pause(0.5)
disp("Conexión inicializada")
end

function stop_program()                            % Terminamos el programa
    global B
    home();
    fclose(B);                                        
end

function valu = read_encoder(espera)               % Leemos el cambio en el encoder
    global B
    fwrite(B,'d');
    pause(espera);
    valu = 20*fread(B,1,'int8');
end

function home(theta1_min, theta2_min)              % Mandamos servos a home
    global B
    global theta_1
    global theta_2
    global actual_state;
    theta_1 = theta1_min;
    theta_2 = theta2_min;
    fwrite(B,'a')
    pause(0.02)
    actual_state = [1,1];
end

function servo_1(angle)                             % Posicionamos el servo 1
    global B
    fwrite(B,'b')
    pause(0.02)
    fwrite(B,angle)
    pause(0.02)
end

function servo_2(angle)                             % Posicionamos el servo 2
    global B
    fwrite(B,'c')
    pause(0.02)
    fwrite(B,angle)
    pause(0.02)
end

function set_servos(action, theta1_max, theta1_min, theta2_max, theta2_min, N_states_s1, N_states_s2)
    global theta_1
    global theta_2
    delta_s1 = round((theta1_max-theta1_min)/N_states_s1);
    delta_s2 = round((theta2_max-theta2_min)/N_states_s2);
    
    switch action
        case 1                                  % Incrementar S1
            theta_1 = theta_1 + delta_s1;
            if theta_1 >= theta1_max   
                theta_1 = theta1_max;  
            end 
            servo_1(theta_1);
        case 2                                  % Decrementar S1
            theta_1 = theta_1 - delta_s1;
            if theta_1 <= theta1_min   
                theta_1 = theta1_min;  
            end 
            servo_1(theta_1);
        case 3                                  % Incrementar S2
            theta_2 = theta_2 + delta_s2;
            if theta_2 >= theta2_max   
                theta_2 = theta2_max;  
            end 
            servo_2(theta_2);
        case 4                                  % Decrementar S2
            theta_2 = theta_2 - delta_s2;
            if theta_2 <= theta2_min   
                theta_2 = theta2_min;  
            end 
            servo_2(theta_2);
        otherwise
    end
end

function set_action(action, N_states_s1, N_states_s2)       % Ejecuta una acción  y actualiza el estado actual
    global B    
    global actual_state;
    global next_state;
    switch action
        case 1                                              
            if actual_state(1) < N_states_s1  
                next_state(1) = actual_state(1) + 1;
            end 
        case 2
            if actual_state(1) > 1   
                next_state(1) = actual_state(1) - 1; 
            end 
        case 3
            if actual_state(2) < N_states_s2  
                next_state(2) = actual_state(2) + 1; 
            end 
        case 4
            if actual_state(2) > 1  
                next_state(2) = actual_state(2) - 1; 
            end 
        otherwise
    end
    %%% Enviamos la acción al robot para desplegarla en LCD
    fwrite(B,'g');
    pause(0.02);
    fwrite(B,next_state(1));
    pause(0.02);
    fwrite(B,next_state(2));
    
end     

function [value,action] = get_maxQ(state, q_values)            % Devuelve el máximo Q valor de la tabla y su acción correspndiente
    [value,action] = max(q_values(state(1),state(2),:), [], 3, 'omitnan');   
end

function e_greedy_action = soft_greedy_action(epsilon, action, N_actions, q_values, state)                 % Aplica exploración por soft e-greedy y devuelve una acción aleatoria 
    check = NaN;
    if rand() < (1-epsilon)                      % Número aleatorio entre 0 y 1
        e_greedy_action = action;
    else
        while (isnan(check))
            e_greedy_action = randi(N_actions);      % Número enteo aleatorio entre 1 y N_actions
            check = q_values(state(1),state(2),e_greedy_action);
        end
    end   
end

