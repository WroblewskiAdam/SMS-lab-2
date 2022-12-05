lambda=1; 
D=20; %horyzont dynamiki
N=20 %horyzont predykcji
N_u=20; %horyzont sterowania 
s = linspace(1,20,20)
M = zeros(N, N_u);
for column=1:N_u
    for row=1:N
        if row - column + 1 >= 1
            M(row, column) = s(row - column + 1);
        else
            M(row, column) = 0;
        end
    end
end

 K = (M'*M+lambda*eye(N_u))^(-1)*M';

M_p = zeros(N, D-1);
for column=1:(D-1)
    for row=1:N
        if row + column > D
            M_p(row, column) = s(D) - s(D-1);
        else
            M_p(row, column) = s(row + column) - s(column);
        end
    end
end
Ke=sum(K(1,:))         %współczynnik Ke
Ku=K(1,:)*M_p;          %współczynnik Ku


% allOneString = sprintf('%.0f,' , n);
% allOneString = allOneString(1:end-1)

strjoin(arrayfun(@(x) num2str(x),Ku,'UniformOutput',false),'f, ')