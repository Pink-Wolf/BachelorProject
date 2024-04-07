const DATABASE_URL = "http://localhost:5042/api/"

async function getJson(path) {
    const response = await fetch(path, {
        method: 'GET',
        cache: 'no-cache',
        headers: {
            'Content-Type': 'application/json',
        }
    })
    if (!response.ok) return null
    return await response.json()
}

export async function getEntity(project, header, name) {
    const entity = await getJson(`${DATABASE_URL}${project}/${header}/${name}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
        header: header,
        name: name,
    }
}
export async function getHeader(project, header) {
    const entity = await getJson(`${DATABASE_URL}${project}/${header}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
        header: header,
    }
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}overview`)
}
