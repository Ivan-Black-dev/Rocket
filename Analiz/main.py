with open('DATALOG.TXT') as file:
    data = file.readlines()
data = [ tuple(i.replace('\n', '').split(' ')) for i in data ]
data = [ tuple([float(i[0]), int(i[1])]) for i in data ]

maxHigh = (0, 0)

for dot in data:
    if dot[0] > maxHigh[0]:
        maxHigh = dot
print(f'Максимальная высота подъёма: {maxHigh[0]} м \nЧерез {maxHigh[1]*10**-3} секнуд после включения устройства')
