<?php

function hardwoods_jbrowse_add_track_form($form, &$form_state, $instance_id) {
  if (empty(hardwoods_jbrowse_get_instance($instance_id))) {
    drupal_not_found();
    return [];
  }

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Track Label'),
    '#description' => t('This will appear on the sidebar.'),
    '#required' => TRUE,
  ];

  $form['instance_id'] = [
    '#type' => 'hidden',
    '#value' => $instance_id,
  ];

  $form['data'] = [
    '#type' => 'fieldset',
    '#title' => t('Track Files'),
  ];

  $form['data']['track_type'] = [
    '#type' => 'select',
    '#description' => t(
      'See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'
    ),
    '#required' => TRUE,
    '#title' => t('Track Type'),
    '#options' => drupal_map_assoc(
      [
        'FeatureTrack',
        'CanvasFeatures',
        'HTMLFeatures',
      ]
    ),
  ];

  $form['data']['file_type'] = [
    '#type' => 'select',
    '#title' => t('File Type'),
    '#options' => drupal_map_assoc(['gff', 'bed', 'gbk']),
    '#description' => t(
      'See http://gmod.org/wiki/JBrowse_Configuration_Guide#flatfile-to-json.pl for more info.'
    ),
    '#required' => TRUE,
  ];

  $form['data']['file'] = [
    '#type' => 'file',
    '#title' => t('Reference Sequence FASTA File'),
  ];

  $form['data']['file_path'] = [
    '#type' => 'textfield',
    '#title' => t('- OR Path to File on Server -'),
    '#description' => t(
      'This path will be ignored if a file is provided above. Ex: sites/default/files/file.fasta or /data/file.fasta'
    ),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Add New Track',
  ];

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function hardwoods_jbrowse_add_track_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $file = $_FILES['files']['tmp_name']['file'];
  $local_file = isset($values['file_path']) ? $values['file_path'] : NULL;

  if (empty($file) && empty($local_file)) {
    form_set_error(
      'file',
      'Please provide a local file path or upload a new file.'
    );
  }
  elseif (empty($file) && !empty($local_file)) {
    if (!file_exists($local_file)) {
      form_set_error('file_path', 'The file path provided does not exist.');
    }
  }
  else {
    $file = hardwoods_jbrowse_upload_file('file');
    if (!$file) {
      form_set_error('file', 'Unable to upload file');
    }
    else {
      $form_state['values']['uploaded_file'] = $file;
    }
  }
}

/**
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 */
function hardwoods_jbrowse_add_track_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  $file = isset($values['file_path']) ? $values['file_path'] : NULL;
  if (!empty($values['uploaded_file'])) {
    $file = $values['uploaded_file'];
  }

  $instance = hardwoods_jbrowse_get_instance($values['instance_id']);

  $track_id = hardwoods_jbrowse_create_track(
    $instance,
    [
      'label' => $values['label'],
      'track_type' => $values['track_type'],
      'file_type' => $values['file_type'],
      'file' => $file,
      'created_at' => time(),
    ]
  );

  tripal_add_job(
    'Add JBrowse track to ' . $instance->title,
    'hardwoods_jbrowse',
    'hardwoods_jbrowse_add_track_to_instance',
    [$track_id],
    $user->uid
  );

  drupal_goto('admin/hardwoods_jbrowse/instances/' . $instance->id);
}

/**
 * @param array $form
 * @param array $form_state
 * @param int $track_id
 *
 * @return array
 * @throws \Exception
 */
function hardwoods_jbrowse_manage_track_form($form, &$form_state, $track_id) {
  $track = hardwoods_jbrowse_get_track($track_id);
  if (empty($track)) {
    drupal_not_found();
    return $form;
  }

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#description' => t('Change the label'),
  ];

  $instance = hardwoods_jbrowse_get_instance($track->instance_id);
  $data = print_r(hardwoods_jbrowse_get_json($instance), TRUE);
  $form['d'] = [
    '#type' => 'markup',
    '#markup' => '<pre>'.$data.'</pre>',
  ];

  return $form;
}
