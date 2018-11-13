<?php

/**
 * @param $form
 * @param $form_state
 * @param int $instance_id The Instance ID
 */
function hardwoods_jbrowse_delete_instance_form(
  $form,
  &$form_state,
  $instance_id
) {
  $instance = hardwoods_jbrowse_get_instance($instance_id);
  if (empty($instance)) {
    drupal_not_found();
    return [];
  }

  $form['confirm_message'] = [
    '#type' => 'item',
    '#markup' => 'Are you sure you want to permanently delete ' . $instance->title . ' JBrowse instance?',
  ];

  $form['instance_id'] = [
    '#type' => 'hidden',
    '#value' => $instance_id,
  ];

  $form['delete'] = [
    '#type' => 'submit',
    '#value' => 'Delete Instance',
  ];

  $form['cancel'] = [
    '#type' => 'markup',
    '#markup' => l(
      'Cancel',
      'admin/hardwoods_jbrowse/instances'
    ),
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function hardwoods_jbrowse_delete_instance_form_validate($form, &$form_state) {
  $instance_id = $form_state['values']['instance_id'] ?? NULL;
  if (is_null($instance_id)) {
    form_set_error('instance_id', 'Please provide a valid instance');
  }

  $instance = hardwoods_jbrowse_get_instance($instance_id);
  if (empty($instance)) {
    form_set_error('instance_id', 'Please provide a valid instance');
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_delete_instance_form_submit($form, &$form_state) {
  $instance_id = $form_state['values']['instance_id'];
  $instance = hardwoods_jbrowse_get_instance($instance_id);
  hardwoods_jbrowse_delete_instance($instance);

  $settings = hardwoods_jbrowse_get_settings();

  drupal_set_message('Instance deleted successfully');
  drupal_set_message(
    'Please delete data directory located at: ' . $settings['data_dir'] . '/' . hardwoods_jbrowse_make_slug(
      $instance->title
    ),
    'warning'
  );

  drupal_goto('admin/hardwoods_jbrowse/instance/' . $instance_id);
}
