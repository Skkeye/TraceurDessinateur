
import math

# max_steps: Le plus grand nombre de pas demandé
# req_steps: Le nombre de pas demandés par le moteur
# ratio: Le ratio entre le nombre de pas demandé et le nombre de pas effectué
# this_step: Le pas actuel
# steps_taken: Le nombre de pas effectué par le moteur
# doesStep: Vérifie si le moteur doit effectuer un pas
# msg: Le message à envoyer au moteur

def ratio(max_steps, req_steps) -> float:
    return max_steps / (req_steps + 1)
    
def doesStep(step_ratio, this_step, steps_taken, req_steps) -> bool:
    stepVal = math.ceil(steps_taken * step_ratio)
    return stepVal == this_step and steps_taken <= req_steps

def loop(ratio):
    count = 1
    for i in range(1, max_steps + 1):
        # section qui peut etre repetee pour tous les moteurs
        if (doesStep(ratio, i, count, req_steps)):
            count += 1 # prendre un pas
        else:
            pass # ne pas prendre de pas
        # fin de la section

def main():
    msg = loop(ratio(max_steps, req_steps))

    print(msg) # Affiche l'ordre des pas à effectuer pour le moteur
    print(msg[::-1]) # Affiche l'ordre des pas à effectuer pour le moteur en sens inverse (pour comparer la symétrie)
    print(msg == msg[::-1]) # Vérifie si l'ordre des pas est symétrique
    print(len(msg)) # Affiche le nombre de pas total à effectuer
    print(len(msg.replace('0', ''))) # Affiche le nombre de pas effectués par le moteur


if __name__ == "__main__":
    max_steps = int(input("Max: "))
    req_steps = int(input("Req: "))
    main()
