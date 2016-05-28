///////////////////////////////////////////////////////////////////////////////
// Name:        wx/secretstore.h
// Purpose:     Storing and retrieving secrets using OS-provided facilities.
// Author:      Vadim Zeitlin
// Created:     2016-05-27
// Copyright:   (c) 2016 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SECRETSTORE_H_
#define _WX_SECRETSTORE_H_

#include "wx/defs.h"

#if wxUSE_SECRETSTORE

class wxSecretStoreImpl;
class wxSecretValueImpl;

// ----------------------------------------------------------------------------
// Represents a secret value, e.g. a password string.
// ----------------------------------------------------------------------------

// This is an immutable value-like class which tries to ensure that the secret
// value will be wiped out from memory once it's not needed any more.
class WXDLLIMPEXP_BASE wxSecretValue
{
public:
    // Creates an empty secret value (not the same as an empty password).
    wxSecretValue() : m_impl(NULL) { }

    // Creates a secret value from the given data.
    wxSecretValue(size_t size, const void *data);

    wxSecretValue(const wxSecretValue& other);
    wxSecretValue& operator=(const wxSecretValue& other);

    ~wxSecretValue();

    // Check if a secret is not empty.
    bool IsOk() const { return m_impl != NULL; }

    // Compare with another secret.
    bool operator==(const wxSecretValue& other) const;
    bool operator!=(const wxSecretValue& other) const
    {
        return !(*this == other);
    }

    // Get the size, in bytes, of the secret data.
    size_t GetSize() const;

    // Get read-only access to the secret data.
    //
    // Don't assume it is NUL-terminated, use GetSize() instead.
    const void *GetData() const;

    // Erase the given area of memory overwriting its presumably sensitive
    // content.
    static void Wipe(size_t size, void *data);

private:
    // This ctor is only used by wxSecretStore and takes ownership of the
    // provided existing impl pointer.
    explicit wxSecretValue(wxSecretValueImpl* impl) : m_impl(impl) { }

    wxSecretValueImpl* m_impl;

    friend class wxSecretStore;
};

// ----------------------------------------------------------------------------
// A collection of secrets, sometimes called a key chain.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxSecretStore
{
public:
    // Returns the default secrets collection to use.
    //
    // Currently this is the only way to create a secret store object. In the
    // future we could add more factory functions to e.g. create non-persistent
    // stores or allow creating stores corresponding to the native facilities
    // being used (e.g. specify schema name under Linux or a SecKeychainRef
    // under OS X).
    static wxSecretStore GetDefault();

    // This class has no default ctor, use GetDefault() instead.

    // But it can be copied, a copy refers to the same store as the original.
    wxSecretStore(const wxSecretStore& store);

    // Dtor is not virtual, this class is not supposed to be derived from.
    ~wxSecretStore();


    // Check if this object is valid.
    bool IsOk() const { return m_impl != NULL; }


    // Store a secret.
    //
    // The service name should be user readable and unique.
    //
    // If a secret with the same service name and user already exists, it will
    // be overwritten with the new value.
    //
    // Returns false after logging an error message if an error occurs,
    // otherwise returns true indicating that the secret has been stored.
    bool Save(const wxString& service,
              const wxString& user,
              const wxSecretValue& secret);

    // Look up a secret.
    //
    // If no such secret is found, an empty value is returned, but no error is
    // logged (however an error may still be logged if some other error occurs).
    // If more than one secret matching the parameters exist, only one
    // arbitrarily chosen of them is returned (notice that it's impossible to
    // get into such situation using this API only).
    wxSecretValue Load(const wxString& service, const wxString& user) const;

    // Delete a previously stored secret.
    //
    // If there is more than one matching secret, all of them are deleted.
    //
    // If any secrets were deleted, returns true. Otherwise returns false and
    // logs an error if any error other than not finding any matching secrets
    // occurred.
    bool Delete(const wxString& service, const wxString& user);

private:
    // Ctor takes ownership of the passed pointer.
    explicit wxSecretStore(wxSecretStoreImpl* impl) : m_impl(impl) { }

    wxSecretStoreImpl* const m_impl;
};

#endif // wxUSE_SECRETSTORE

#endif // _WX_SECRETSTORE_H_
