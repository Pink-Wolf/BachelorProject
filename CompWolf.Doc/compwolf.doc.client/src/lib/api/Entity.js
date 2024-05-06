import betterEncodeURIComponent from "../betterEncodeURIComponent"

const DATABASE_URL = "http://localhost:5042/"

async function getJson(path) {
    const response = await fetch(path, {
        method: 'GET',
        cache: `no-cache`,
        headers: {
            'Content-Type': 'application/json',
        },
    })
    if (!response.ok) throw new Error(`Could not get data from ${path}\n${response.status}: ${response.statusText}`)
    return await response.json()
}

export async function getExample(name) {
    const example = await getJson(`${DATABASE_URL}examples/${name}`)
    return example
}

export async function getProject(project) {
    const entity = await getJson(`${DATABASE_URL}api/${project}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
    }
}
export async function getHeader(project, header) {
    const entity = await getJson(`${DATABASE_URL}api/${project}/${header}`)
    if (entity == null) return null
    return {
        ...entity,
        project: project,
        header: header,
    }
}
export async function getEntity(project, header, name) {
    const entity = await getJson(`${DATABASE_URL}api/${project}/${header}/${name}`)
    if (entity == null) return null
    const returnVal = {
        ...entity,
        project: project,
        header: header,
        name: name,
        owners: [],
        example: (entity.example === undefined) ? undefined : await getExample(entity.example)
    }
    switch (returnVal.type) {
        case `class`:
            if (returnVal.hasOwnProperty("constructor")) {
                if (returnVal.copyable && returnVal.movable) {
                    returnVal.constructor.briefDescription = `${returnVal.name} is copyable and movable.`
                    returnVal.constructor.detailedDescription = ``
                }
                else if (returnVal.copyable && !returnVal.movable) {
                    returnVal.constructor.briefDescription = `${returnVal.name} is copyable, but not movable.`
                    returnVal.constructor.detailedDescription = `${returnVal.name} is not movable.`
                }
                else if (!returnVal.copyable && returnVal.movable) {
                    returnVal.constructor.briefDescription = `${returnVal.name} is not copyable, but is movable.`
                    returnVal.constructor.detailedDescription = `${returnVal.name} is not copyable.`
                }
                else if (!returnVal.copyable && !returnVal.movable) {
                    returnVal.constructor.briefDescription = `${returnVal.name} is not copyable nor movable.`
                    returnVal.constructor.detailedDescription = `${returnVal.name} is not copyable nor movable.`
                }
            }
        default: return returnVal
    }
}

export async function getOverview() {
    return await getJson(`${DATABASE_URL}api/overview`)
}

export async function getPathTo(name) {
    const overview = await getOverview()

    let memberSplitterIndex = name.indexOf("::")
    let isMember = memberSplitterIndex >= 0;
    let memberName = !isMember ? "" : name.substring(memberSplitterIndex + 2, name.length).split("::").map(x => betterEncodeURIComponent(x)).join("/");
    if (isMember) {
        name = name.substring(0, memberSplitterIndex)

        if (name == `std`) return undefined
    }

    var path = undefined
    overview.projects?.find(project => {
        if (project.name === name) path = `/api/${project.name}/`
        project.headers?.find(header => {
            if (header.name === name) path = `/api/${project.name}/${header.name}/`
            header.entities?.find(entity => {
                if (entity.name === name) path = `/api/${project.name}/${header.name}/${betterEncodeURIComponent(entity.name)}/${memberName}`

                return (path !== undefined)
            })
            return (path !== undefined)
        })
        return (path !== undefined)
    })

    if (path === undefined) {
        console.log(`Could not find path to ${name}`)
        return undefined
    }

    return path
}
