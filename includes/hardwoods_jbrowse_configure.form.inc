<?php
/**
 * The settings form.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function hardwoods_jbrowse_configure_form($form, &$form_state) {
  $form['message'] = [
    '#type' => 'markup',
    '#prefix' => '<p>',
    '#markup' => t(
      'This form allows administrators to control 
      where JBrowse lives on the server.'
    ),
    '#suffix' => '</p>',
  ];

  $settings = hardwoods_jbrowse_get_settings();

  $form['data_dir'] = [
    '#title' => t('Data Directory'),
    '#description' => t(
      'Absolute path to where data directories should live. 
       This directory must be accessible by the apache user.'
    ),
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#default_value' => $settings['data_dir'],
    '#required' => TRUE,
  ];

  $form['link'] = [
    '#title' => t('Path to JBrowse\'s Index File'),
    '#description' => t(
      'Path to index.html that JBrowse provides.
       We suggest using tools/jbrowse.'
    ),
    '#type' => 'textfield',
    '#default_value' => $settings['link'],
    '#maxlength' => 255,
    '#required' => TRUE,
  ];

  $form['bin_path'] = [
    '#title' => t('Path to JBrowse\'s bin Directory'),
    '#description' => t(
      'The absolute path to the bin directory that JBrowse provides.
       We suggest using PATH_TO_DRUPAL/tools/jbrowse/bin.'
    ),
    '#type' => 'textfield',
    '#default_value' => $settings['bin_path'],
    '#maxlength' => 255,
    '#required' => TRUE,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save Settings',
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function hardwoods_jbrowse_configure_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // DATA DIR
  if (!file_exists($values['data_dir'])) {
    form_set_error('data_dir', 'The data directory does not exist.');
  }
  elseif (!is_writable($values['data_dir']) || !is_dir($values['data_dir'])) {
    form_set_error('data_dir', 'The data directory is not writeable.');
  }

  // BIN PATH
  $bin_path = $values['bin_path'];
  if (!file_exists($bin_path)) {
    form_set_error(
      'bin_path',
      'The bin directory (' . $bin_path . ') does not exist'
    );
  }
  elseif (!file_exists($bin_path . '/prepare-refseqs.pl')) {
    form_set_error(
      'bin_path',
      'The bin directory does not contain the required scripts.
       Please make sure the prepare-refseqs.pl script exists in ' . $bin_path
    );
  }

  $link_path = DRUPAL_ROOT . '/' . trim($values['link'], '/') . '/index.html';
  if (!file_exists($link_path)) {
    form_set_error('link', 'index.html does not exist in ' . $link_path);
  }
}

/**
 * @param $form
 * @param $form_state
 */
function hardwoods_jbrowse_configure_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $bin_path = rtrim($values['bin_path'], '/');

  $settings = [
    'data_dir' => rtrim($values['data_dir'], '/'),
    'bin_path' => $bin_path,
    'link' => $values['link'],
  ];

  hardwoods_jbrowse_save_settings($settings);

  drupal_set_message('Settings saved successfully');
}
