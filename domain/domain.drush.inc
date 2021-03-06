<?php

/**
 * @file
 * Drush commands for Domain Access.
 */

use Drupal\domain\DomainInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Component\Utility\String;

/**
 * Implements hook_drush_command().
 */
function domain_drush_command() {
  $items = array();

  $items['domain-list'] = array(
    'description' => 'List active domains for the site.',
    'examples' => array(
      'drush domain-list',
      'drush domains',
    ),
    'aliases' => array('domains'),
  );
  $items['domain-add'] = array(
    'description' => 'Add a new domain to the site.',
    'examples' => array(
      'drush domain-add example.com \'My Test Site\'',
      'drush domain-add example.com \'My Test Site\' --inactive=1 --https==1',
      'drush domain-add example.com \'My Test Site\' --weight=10',
    ),
    'arguments' => array(
      'hostname' => 'The domain hostname to register (e.g. example.com).',
      'name' => 'The name of the site (e.g. Domain Two).',
    ),
    'options' => array(
      'inactive' => 'Set the domain to inactive status if set.',
      'https' => 'Use https protocol for this domain if set.',
      'weight' => 'Set the order (weight) of the domain.',
      'is_default' => 'Set this domain as the default domain.'
    ),
  );
  $items['domain-delete'] = array(
    'description' => 'Delete a domain from the site.',
    'examples' => array(
      'drush domain-delete example.com',
      'drush domain-delete 1',
    ),
    'arguments' => array(
      'domain' => 'The numeric id or hostname of the domain to delete.',
    ),
  );
  $items['domain-test'] = array(
    'description' => 'Tests domains for proper response. If run from a subfolder, you must specify the --uri.',
    'examples' => array(
      'drush domain-test',
      'drush domain-test example.com',
      'drush domain-test 1',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to test. If no value is passed, all domains are tested.',
    ),
    'options' => array(
      'base_path' => 'The subdirectory name if Drupal is installed in a folder other than server root.',
    ),
  );
  $items['domain-default'] = array(
    'description' => 'Sets the default domain. If run from a subfolder, you must specify the --uri.',
    'examples' => array(
      'drush domain-default example.com',
      'drush domain-default 1',
      'drush domain-default 1 --skip_check=1',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
    ),
    'options' => array(
      'skip_check' => 'Bypass the domain response test.'
    ),
  );
  $items['domain-disable'] = array(
    'description' => 'Sets a domain status to off.',
    'examples' => array(
      'drush domain-disable example.com',
      'drush domain-disable 1',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
    ),
  );
  $items['domain-enable'] = array(
    'description' => 'Sets a domain status to on.',
    'examples' => array(
      'drush domain-disable example.com',
      'drush domain-disable 1',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
    ),
  );
  $items['domain-name'] = array(
    'description' => 'Changes a domain name.',
    'examples' => array(
      'drush domain-name example.com Foo',
      'drush domain-name 1 Foo',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
      'name' => 'The name to use for the domain.',
    ),
  );
  $items['domain-machine-name'] = array(
    'description' => 'Changes a domain name.',
    'examples' => array(
      'drush domain-name example.com Foo',
      'drush domain-name 1 Foo',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
      'name' => 'The name to use for the domain.',
    ),
  );
  $items['domain-scheme'] = array(
    'description' => 'Changes a domain scheme.',
    'examples' => array(
      'drush domain-scheme example.com',
      'drush domain-scheme 1 Foo',
    ),
    'arguments' => array(
      'domain_id' => 'The numeric id or hostname of the domain to make default.',
    ),
  );
  $items['generate-domains'] = array(
    'description' => 'Generate domains for testing.',
    'arguments' => array(
      'primary' => 'The primary domain to use. This will be created and used for *.example.com hostnames.',
    ),
    'options' => array(
      'count' => 'The count of extra domains to generate. Default is 15.',
      'empty' => 'Pass empty=1 to truncate the {domain} table before creating records.'
    ),
    'examples' => array(
      'drush domain-generate example.com',
      'drush domain-generate example.com --count=25',
      'drush domain-generate example.com --count=25 --empty=1',
      'drush gend',
      'drush gend --count=25',
      'drush gend --count=25 --empty=1',
    ),
    'aliases' => array('gend'),
  );
  return $items;
}

/**
 * Implements hook_drush_help().
 */
function domain_drush_help($section) {
  $items = domain_drush_command();
  $name = str_replace('domain:', '', $section);
  if (isset($items[$name])) {
    return dt($items[$name]['description']);
  }
}

/**
 * Shows the domain list.
 */
function drush_domain_list() {
  $domains = domain_load_and_sort(NULL, TRUE);

  if (empty($domains)) {
    drush_print(dt('No domains have been created. Use drush domain-add to create one.'));
    return;
  }

  $header = array(
    'weight' => dt('Weight'),
    'name' => dt('Name'),
    'hostname' => dt('Hostname'),
    'scheme' => dt('Scheme'),
    'status' => dt('Status'),
    'is_default' => dt('Default'),
    'domain_id' => dt('Domain Id'),
    'id' => dt('Machine name'),
  );
  $rows = array(array_values($header));
  foreach ($domains as $domain) {
    $row = array();
    foreach ($header as $key => $name) {
      $row[] = String::checkPlain($domain->get($key));
    }
    $rows[] = $row;
  }

  drush_print_table($rows, TRUE);
}

/**
 * Generates a list of domains for testing.
 *
 * In my environment, I name hostnames one.* two.* up to ten. I also use
 * foo.* bar.* and baz.*. We also want a non-hostname here and use
 * myexample.com.
 *
 * The script may also add test1, test2, test3 up to any number to test a
 * large number of domains. This test is mostly for UI testing.
 *
 * @param $primary
 *   The root domain to use for domain creation.
 *
 * @return
 *   A list of the domains created.
 */
function drush_domain_generate_domains($primary = 'example.com') {
  // Check the number of domains to create.
  $count = drush_get_option('count');
  $domains = domain_load_multiple(NULL, TRUE);
  if (empty($count)) {
    $count = 15;
  }
  // Ensure we don't duplicate any domains.
  $existing = array();
  if (!empty($domains)) {
    foreach ($domains as $domain) {
      $existing[] = $domain->getHostname();
    }
  }
  // Set up one.* and so on.
  $names = array(
    'one',
    'two',
    'three',
    'four',
    'five',
    'six',
    'seven',
    'eight',
    'nine',
    'ten',
    'foo',
    'bar',
    'baz',
  );
  // Set the creation array.
  $new = array($primary);
  foreach ($names as $name) {
    $new[] = $name . '.' . $primary;
  }
  // Include a non hostname.
  $new[] = 'my' . $primary;
  // Filter against existing so we can count correctly.
  $prepared = array();
  foreach ($new as $key => $value) {
    if (!in_array($value, $existing)) {
      $prepared[] = $value;
    }
  }
  // Add any test domains.
  if ($count > 15 || empty($prepared)) {
    // @TODO fix this logic.
    $start = 1;
    $j = count($prepared);
    for ($i = $start + 1; $j <= $count; $i++) {
      $prepared[] = 'test' . $i . '.' . $primary;
      $j++;
    }
  }
  // Get the initial item weight for sorting.
  $start_weight = count($domains);
  $prepared = array_slice($prepared, 0, $count);
  foreach ($prepared as $key => $item) {
    $hostname = strtolower($item);
    $values = array(
      'name' => ($item != $primary) ? ucwords(str_replace(".$primary", '', $item)) : \Drupal::config('system.site')->get('name'),
      'hostname' => $hostname,
      'scheme' => 'http',
      'status' => 1,
      'weight' => ($item != $primary) ? $key + $start_weight + 1 : -1,
      'is_default' => 0,
      'id' => domain_machine_name($hostname),
    );
    $domain = domain_create(FALSE, $values);
    domain_drush_create($domain);
  }

  // If nothing created, say so.
  if (empty($new)) {
    drush_print(dt('No new domains were created.'));
  }
}

/**
 * Validates the domain generation script.
 *
 * @param $primary
 *   The root domain to use for domain creation.
 */
function drush_domain_generate_domains_validate($primary = 'example.com') {
  if ($empty = drush_get_option('empty')) {
    db_query("TRUNCATE TABLE {domain}");
  }
  return;
  // TODO: Update this validation.
  $error = domain_valid_domain($primary);
  if (!empty($error)) {
    return drush_set_error('domain', $error);
  }
}

/**
 * Adds a new domain.
 *
 * @param $hostname
 *   The domain name to register.
 * @param $name
 *   The name to use for this domain.
 *
 * @return
 *   The domain created or an error message.
 */
function drush_domain_add($hostname, $name) {
  $start_weight = count(domain_load_multiple()) + 1;
  $hostname = strtolower($hostname);
  $values = array(
    'hostname' => $hostname,
    'name' => $name,
    'status' => (!drush_get_option('invalid')) ? 1 : 0,
    'scheme' => (!drush_get_option('https')) ? 'http' : 'https',
    'weight' => ($weight = drush_get_option('weight')) ? $weight : $start_weight + 1,
    'is_default' => ($is_default = drush_get_option('is_default')) ? $is_default : 0,
    'id' => domain_machine_name($hostname),
  );
  $domain = domain_create(FALSE, $values);
  /* TODO: Fix this check.
  if (!empty($domain->is_default)) {
    $domain->checkResponse();
    drush_print($domain->response);
  }
  */
  domain_drush_create($domain);
}

/**
 * Validates the domain add script.
 *
 * @param $hostname
 *   The domain name to register.
 * @param $name
 *   The name to use for this domain.
 */
function drush_domain_add_validate($hostname, $name) {
  $error = domain_drush_validate_domain($hostname);
  if (!empty($error)) {
    return drush_set_error('domain', $error);
  }
}

/**
 * Creates a domain record.
 *
 * @param Drupal\domain\DomainInterface $domain
 *   A domain entity.
 */
function domain_drush_create(DomainInterface $domain) {
  $domain->save();
  if ($domain->getDomainId()) {
    drush_print(dt('Created @name at @domain.', array('@name' => $domain->label(), '@domain' => $domain->getHostname())));
  }
  else {
    drush_print(dt('The request could not be completed.'));
  }
}

/**
 * Validates a domain.
 *
 * @param $hostname
 *   The domain name to validate for syntax and uniqueness.
 *
 * @return
 *   An array of errors encountered.
 *
 * @see domain_validate()
 */
function domain_drush_validate_domain($hostname) {
  return;
  // TODO: Restore this validation.
  $error = domain_validate($hostname);
  $output = '';
  foreach ($error as $msg) {
    $output .= $msg;
  }
  return $output;
}

/**
 * Deletes a domain record.
 *
 * @param $argument
 *   The domain_id to delete. Pass 'all' to delete all records.
 */
function drush_domain_delete($argument = NULL) {
  if (is_null($argument)) {
    drush_set_error('domain', dt('You must specify a domain to delete.'));
  }
  if ($argument == 'all') {
    $domains = domain_load_multiple(NULL, TRUE);
    if (empty($domains)) {
      drush_print(dt('There are no domains to delete.'));
      return;
    }
    $content = drush_choice(array(1 => dt('Delete all domains')), dt('This action may not be undone. Continue?'), '!value');
    if (empty($content)) {
      return;
    }
  }
  // Resolve the domain.
  elseif ($domain = drush_domain_get_from_argument($argument)) {
    if ($domain->isDefault()) {
      return drush_set_error('domain', dt('The primary domain may not be deleted. Use drush domain-default to set a new default domain.'));
    }
  }
  else {
    return;
  }
  foreach ($domains as $domain) {
    $domain->delete();
    drush_print(dt('Domain record @domain deleted.', array('@domain' => $domain->getHostname())));
  }

  return;

  // TODO: Set options for re-assigning content.
  $list = domain_load_multiple(NULL, TRUE);
  $options = array('0' => dt('Do not reassign'));
  foreach ($list as $data) {
    if ($data->id() != $domain->id()) {
      $options[$data->getDomainId()] = $data->getHostname();
    }
  }
  $content = drush_choice($options, dt('Reassign content to:'), '!value');
  if (empty($content)) {
    return;
  }
  $users = drush_choice($options, dt('Reassign users to:'), '!value');
  if (empty($users)) {
    return;
  }
  $values['domain_access'] = (!empty($content)) ? $content : 'none';
  $values['domain_editor'] = (!empty($content)) ? $users : 'none';

  domain_delete($domain, $values);

  drush_print(dt('Domain record deleted.'));
}

/**
 * Tests a domain record for the proper HTTP response.
 *
 * @param $argument
 *   The domain_id to test. Passing no value tests all records.
 */
function drush_domain_test($argument = NULL) {
  // TODO: This won't work in a subdirectory without a parameter.
  if ($base_path = drush_get_option('base_path')) {
    $GLOBALS['base_path'] = '/' . $base_path . '/';
  }
  if (is_null($argument)) {
    $domains = domain_load_multiple(NULL, TRUE);
  }
  else {
    if ($domain = drush_domain_get_from_argument($argument)) {
      $domains = array($domain);
    }
    else {
      return;
    }
  }
  foreach ($domains as $domain) {
    if ($domain->getResponse() != 200) {
      drush_print(dt('Fail: !error. Please pass a --uri parameter or a --base_path to retest.' , array('!error' => $domain->getResponse())));
    }
    else {
      drush_print(dt('Success: !url tested successfully.', array('!url' => $domain->getPath())));
    }
  }
}

/**
 * Sets the default domain id.
 */
function drush_domain_default($argument) {
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    // TODO: Check for domain response.
    /*
    $check = drush_get_option('skip_check');
    if (empty($check)) {
      $error = domain_check_response($domain, TRUE);
      if ($error) {
        drush_set_error($error);
        drush_print(dt('You may disable this error by passing --skip_check=1.'));
        return;
      }
    }*/
    $domain->saveDefault();
    drush_print(dt('!domain set to primary domain.', array('!domain' => $domain->getHostname())));
  }
}

/**
 * Disables a domain.
 */
function drush_domain_disable($argument) {
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    if ($domain->status()) {
      $domain->disable();
      drush_print(dt('!domain has been disabled.', array('!domain' => $domain->getHostname())));
    }
    else {
      drush_print(dt('!domain is already disabled.', array('!domain' => $domain->getHostname())));
    }
  }
}

/**
 * Enables a domain.
 */
function drush_domain_enable($argument) {
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    if (!$domain->status()) {
      $domain->enable();
      drush_print(dt('!domain has been enabled.', array('!domain' => $domain->getHostname())));
    }
    else {
      drush_print(dt('!domain is already enabled.', array('!domain' => $domain->getHostname())));
    }
  }
}

/**
 * Changes a domain name.
 */
function drush_domain_name($argument, $name) {
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    $domain->saveProperty('name', $name);
  }
}

/**
 * Changes a domain machine_name.
 */
function drush_domain_machine_name($argument, $machine_name) {
  $machine_name = domain_machine_name($machine_name);
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    $results = entity_load_multiple_by_properties('domain', array('machine_name' => $machine_name));
    foreach ($results as $result) {
      if ($result->id() == $machine_name) {
        drush_print(dt('The machine_name @machine_name is being used by domain @hostname.', array('@machine_name' => $machine_name, '@hostname' => $result->getHostname())));
        return;
      }
    }
    $domain->saveProperty('id', $machine_name);
  }
}

/**
 * Changes a domain scheme.
 */
function drush_domain_scheme($argument) {
  // Resolve the domain.
  if ($domain = drush_domain_get_from_argument($argument)) {
    $content = drush_choice(array(1 => dt('http'), 2 => dt('https')), dt('Select the default http scheme:'), '!value');
    if (empty($content)) {
      return;
    }
    $scheme = 'http';
    if ($content == 2) {
      $scheme = 'https';
    }
    $domain->saveProperty('scheme', $scheme);
  }
}

/**
 * Converts a domain string or domain_id to a $domain array.
 *
 * On failure, throws a drush error.
 */
function drush_domain_get_from_argument($argument) {
  $domain = domain_load($argument);
  if (!$domain) {
    $domain = domain_load_hostname($argument);
  }
  if (!$domain) {
    drush_set_error('domain', dt('Domain record not found.'));
    return FALSE;
  }
  return $domain;
}
