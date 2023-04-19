/**
 * @file
 * @brief Les definitions pour manipuler des moteurs steppers
 *
 * Olivier David Laplante -- 8 Avril 2023 : Creation
 */

#pragma once
/**
 * @class StepperWrapper
 * @brief Objet pour simplifier un moteur stepper
 *
 */
class StepperWrapper {
private:
  int dir_pin, step_pin, en_pin, home_pin;
  
  long max_distance_from_origin;
  bool positive_direction;
  bool direction;

  /**
   * @brief Initialise les pins relative au moteur
   */
  void initPins();

public:
  long distance_from_origin;
  /**
   * @brief Le constructeur pour le stepper
   *
   * @param dir La pin de direction
   * @param step La pin de step
   * @param en La pin enable
   * @param home La pin de l'interrupteur de limite
   * @param max_distance La distance maximale en pas
   * @param flip_dir Change la direction par defaut (defaut = false)
   */
  StepperWrapper(int dir, int step, int en, int home, int max_distance,
                 bool flip_dir = false);

  /**
   * @brief Fait un pas
   * 
   * @param force Force le pas meme si on est a la limite
   */
  void step(bool force);

  /**
   * @brief Change l'etat de la pin de direction
   *
   * @param dir La nouvelle direction
   */
  void setDir(bool dir);

  /**
   * @brief Change l'etat de la pin enable
   *
   * @param en L'etat
   */
  void setEnabled(bool en);

  /**
   * @brief Verifie si l'interrupteur est enfonce
   *
   * @return bool Si enfonce
   */
  bool isHome();
};

/**
 * @class ThreeAxisStepper
 * @brief Manipule 3 moteurs stepper simultanement
 *
 */
class ThreeAxisStepper {
private:
  StepperWrapper *stepper_x, *stepper_y, *stepper_z;

  /**
   * @brief Utilise pour repartir egalement les pas des moteurs
   *
   * @param this_step Le nombre de pas que le moteur a fait durant ce
   * deplacement
   * @param count Le nombre de pas de ce deplacement
   * @param steps_moving Le nombre de pas requis pour ce moteur pour ce
   * deplacement
   * @param largest_step_count Le plus grand nombre de pas requis pour de
   * deplacement
   * @return Si le moteur fait un pas
   */
  bool doesStep(int this_step, int count, int steps_moving,
                int largest_step_count);

public:
  /**
   * @brief Le constructeur
   *
   * @param stepperX Pointeur vers le StepperWrapper du moteur de l'axe X
   * @param stepperY Pointeur vers le StepperWrapper du moteur de l'axe Y
   * @param stepperZ Pointeur vers le StepperWrapper du moteur de l'axe Z
   */
  ThreeAxisStepper(StepperWrapper *stepperX, StepperWrapper *stepperY,
                   StepperWrapper *stepperZ);

  /**
   * @brief Execute un deplacement
   *
   * @param deltaX Le nombre de pas pour l'axe X
   * @param deltaY Le nombre de pas pour l'axe Y
   * @param deltaZ Le nombre de pas pour l'axe Z
   */
  void move(int deltaX, int deltaY, int deltaZ);

  /**
   * @brief Remet a 0 les moteurs
   */
  void home();

  /**
   * @brief Fonctions pour activer et desactiver les moteurs
   */
  void disable();
  void enable();
};
