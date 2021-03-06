<?php

/**
 * @file
 * Definition of Drupal\domain\Tests\DomainHooksTest.
 */

namespace Drupal\domain\Tests;
use Drupal\domain\DomainInterface;

/**
 * Tests the domain module hook invocations.
 *
 * @group domain
 */
class DomainHooksTest extends DomainTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('domain', 'domain_test');

  /**
   * Tests domain loading.
   */
  function testHookDomainLoad() {
    // No domains should exist.
    $this->domainTableIsEmpty();

    // Create a domain.
    $this->domainCreateTestDomains();

    // @TODO: We need a new loader?
    $key = domain_machine_name(domain_hostname());

    $domain = domain_load($key);

    // Internal hooks.
    $path = $domain->getPath();
    $url = $domain->getUrl();
    $this->assertTrue(isset($path), format_string('The path property was set to %path by hook_entity_load.', array('%path' => $path)));
    $this->assertTrue(isset($url), format_string('The url property was set to %url by hook_entity_load.', array('%url' => $url)));

    // External hooks.
    $this->assertTrue($domain->foo == 'bar', 'The foo property was set to <em>bar</em> by hook_domain_load.');
  }

}
